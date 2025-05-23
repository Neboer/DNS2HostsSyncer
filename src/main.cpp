#include "configs/parse_args.h"
#include "dns_records/restapi.h"
#include "dns_records/protocol.h"
#include "configs/rrpool_conf.h"
#include "dns_records/record.h"
#include "hosts/hosts_file.h"
#include "configs/parse_config.h"
#include <algorithm>
#include <spdlog/spdlog.h>
#include <curl/curl.h>
#include "converter.h"
#include "diff.hpp"
#include <spdlog/sinks/basic_file_sink.h>
#include "exit_code.hpp"

int main(int argc, char **argv)
{
    try
    {
        d2hs::program_arguments program_args = d2hs::parse_args(argc, argv);
        if (program_args.is_log_file_location_set)
        {
            // spdlog::set_level(spdlog::level::info);
            // spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
            spdlog::info("Logging to file: {}", program_args.log_file_location);
            try
            {
                auto file_logger = spdlog::basic_logger_mt("file_logger", program_args.log_file_location);
                spdlog::set_default_logger(file_logger);
            }
            catch (const spdlog::spdlog_ex &e)
            {
                spdlog::error("Failed to open file for logging: {}", e.what());
                return d2hs::ExitCode::LOG_INIT_FAILURE;
            }
        }

        spdlog::info("use config file: {}", program_args.config_file_location);
        d2hs::program_config program_cfg = d2hs::parse_config_file(program_args.config_file_location);
        curl_global_init(CURL_GLOBAL_DEFAULT);

        d2hs::HostsFile hosts_file = program_args.is_hosts_file_set ? d2hs::HostsFile(program_args.hosts_file_path)
                                                                    : d2hs::HostsFile(program_cfg.hosts_file_path);

        std::vector<d2hs::RRRecord> all_records = {};

        // gather api results.
        for (const auto &rrpool_cfg : program_cfg.rrpool_cfgs)
        {
            spdlog::info("sending API request to endpoint: {}, server_name: {}, zone_name: {}", rrpool_cfg.api_endpoint_url, rrpool_cfg.server_name, rrpool_cfg.zone_name);
            try
            {
                json record_response = d2hs::get_json_from_pdns_server(rrpool_cfg);
                spdlog::info("API request completed.");
                std::vector<d2hs::RRRecord> server_records = d2hs::parse_pdns_response(record_response);
                all_records.insert(all_records.end(), server_records.begin(), server_records.end());
            }
            catch (const std::runtime_error &e)
            {
                spdlog::critical("Failed to process PowerDNS API from endpoint: {}, server_name: {}, zone_name: {}. Error: {}. Exiting.",
                                 rrpool_cfg.api_endpoint_url, rrpool_cfg.server_name, rrpool_cfg.zone_name, e.what());
                curl_global_cleanup();
                return d2hs::ExitCode::API_FAILURE;
            }
        }

        // 将所有的记录转换为hosts行
        std::vector<d2hs::HostsLine> new_hosts_lines = d2hs::convert_records_to_hosts_lines(all_records);
        std::vector<d2hs::HostsLine> current_hosts_lines = hosts_file.get_body_lines();

        // 比对新旧hosts行，如果不同则更新hosts文件。
        diff_result<d2hs::HostsLine> diff_result = diff_lists(current_hosts_lines, new_hosts_lines);

        if (diff_result.only_in_first.size() > 0 || diff_result.only_in_second.size() > 0)
        {
            // 先打印出差异
            spdlog::info("Changes detected in DNS result:");
            if (program_args.is_log_file_location_set)
            {
                for (const auto &line : diff_result.only_in_second)
                {
                    spdlog::info("+ {}", line.hostname);
                }
                for (const auto &line : diff_result.only_in_first)
                {
                    spdlog::info("- {}", line.hostname);
                }
            }
            else
            {
                for (const auto &line : diff_result.only_in_second)
                {
                    spdlog::info("\033[32m+ {}\033[0m", line.hostname);
                }
                for (const auto &line : diff_result.only_in_first)
                {
                    spdlog::info("\033[31m- {}\033[0m", line.hostname);
                }
            }

            // hosts文件内容与服务器响应之间存在差异，更新hosts文件。
            spdlog::info("Detected changes in DNS result.");
            try
            {
                hosts_file.save_with_new_body(new_hosts_lines, program_args.dry_run);
            }
            catch (const std::runtime_error &e)
            {
                spdlog::error("Failed to update hosts file. Error: {}", e.what());
                return d2hs::ExitCode::HOSTS_UPDATE_FAILURE;
            }
        }
        else
        {
            spdlog::info("No changes detected in DNS result, hosts file is up to date.");
        }
        
        curl_global_cleanup();
        return d2hs::ExitCode::SUCCESS;
    }
    catch (const std::exception &e)
    {
        spdlog::critical("An error occurred: {}", e.what());
        curl_global_cleanup();
        return d2hs::ExitCode::OTHER_FAILURE;
    }
}
