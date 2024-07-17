#include "parse_args.h"
#include "restapi.h"
#include "protocol.h"
#include "rrpool_conf.h"
#include "record.h"
#include "hosts_file.h"
#include "parse_config.h"
#include <algorithm>
#include <spdlog/spdlog.h>
#include <restclient-cpp/restclient.h>

std::vector<str> rrrecords_to_hosts_lines(const std::vector<d2hs::RRRecord> &records)
{
    std::vector<str> lines = {};
    for (const auto &record : records)
    {
        try
        {
            lines.push_back(record.to_hosts_line());
        }
        catch (const std::invalid_argument &e)
        {
            spdlog::warn("error when converting record {}/{} to hosts line: {}", record.name, record.value, e.what());
        }
    }
    return lines;
}

bool compare_two_unsorted_vectors(const std::vector<str> &v1, const std::vector<str> &v2)
{
    if (v1.size() != v2.size())
    {
        return false;
    }
    std::vector<str> v1_copy = v1;
    std::vector<str> v2_copy = v2;
    std::sort(v1_copy.begin(), v1_copy.end());
    std::sort(v2_copy.begin(), v2_copy.end());
    return v1_copy == v2_copy;
}

int main(int argc, char **argv)
{
    try
    {
        d2hs::program_arguments program_args = d2hs::parse_args(argc, argv);
        d2hs::program_config program_cfg = d2hs::parse_config_file(program_args.config_file_location);
        // initialize the rest client.
        RestClient::init();
        // find host record in hosts file. Hosts file path is passed by the user or /etc/hosts as default.
        d2hs::HostsFile hosts_file = d2hs::HostsFile(program_args.hosts_file_path);

        std::vector<str> all_server_records_hosts_lines = {};

        // gather api results.
        for (const auto &rrpool_cfg : program_cfg.rrpool_cfgs)
        {
            spdlog::info("sending API request to endpoint: {}, server_name: {}, zone_name: {}", rrpool_cfg.api_endpoint_url, rrpool_cfg.server_name, rrpool_cfg.zone_name);
            try
            {
                json record_response = d2hs::get_json_from_pdns_server(rrpool_cfg);
                spdlog::info("API request completed.");
                std::vector<d2hs::RRRecord> server_records = d2hs::parse_pdns_response(record_response);
                std::vector<str> server_records_hosts_lines = rrrecords_to_hosts_lines(server_records);
                all_server_records_hosts_lines.insert(all_server_records_hosts_lines.end(), server_records_hosts_lines.begin(), server_records_hosts_lines.end());
            }
            catch (const std::runtime_error &e)
            {
                spdlog::error("Failed to process PowerDNS API from endpoint: {}, server_name: {}, zone_name: {}. Error: {}. Skipping this API request.",
                              rrpool_cfg.api_endpoint_url, rrpool_cfg.server_name, rrpool_cfg.zone_name, e.what());
                continue;
            }
        }

        std::vector<str> d2hs_hosts_lines = hosts_file.read_d2hs_lines();

        // for each record, check if it exists in the hosts file's d2fs section.
        auto is_records_changed = !compare_two_unsorted_vectors(all_server_records_hosts_lines, d2hs_hosts_lines);

        if (is_records_changed)
        {
            spdlog::info("Detected changes in DNS result. Updating hosts file...");
            // there are difference between the server's response and the hosts file content.
            // update the hosts file.
            str new_resolve_content = hosts_file.write_d2hs_lines(all_server_records_hosts_lines, program_args.dry_run);
            if (program_args.dry_run)
            {
                std::cout << new_resolve_content;
            }
        }
        else
        {
            spdlog::info("No changes detected in DNS result.");
        }

        return 0;
    }
    catch (const std::exception &e)
    {
        spdlog::critical("An error occurred: {}", e.what());
        return 1;
    }
}