#include "parse_args.h"
#include "restapi.h"
#include "protocol.h"
#include "rrpool_conf.h"
#include "record.h"
#include "hosts_file.h"
#include <algorithm>
#include <spdlog/spdlog.h>
#include <restclient-cpp/restclient.h>

std::vector<str> rrrecords_to_hosts_lines(const std::vector<d2hs::RRRecord> &records)
{
    std::vector<str> lines = {};
    for (const auto &record : records)
    {
        lines.push_back(record.to_hosts_line());
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
    d2hs::program_config program_cfg = d2hs::parse_args(argc, argv);
    // initialize the rest client.
    RestClient::init();
    // find host record in hosts file. Hosts file path is passed by the user or /etc/hosts as default.
    d2hs::HostsFile hosts_file = d2hs::HostsFile(program_cfg.hosts_file_path);

    spdlog::info("sending API request...");
    // api sending from rrpoolcfg passed from command line.
    json record_response = d2hs::get_json_from_pdns_server(program_cfg.rrpool_cfg);
    spdlog::info("API request successful.");
    std::vector<d2hs::RRRecord> server_records = d2hs::parse_pdns_response(record_response);
    std::vector<str> server_records_hosts_lines = rrrecords_to_hosts_lines(server_records);
    std::vector<str> d2hs_hosts_lines = hosts_file.read_d2hs_lines();

    // for each record, check if it exists in the hosts file's d2fs section.
    auto is_records_changed = !compare_two_unsorted_vectors(server_records_hosts_lines, d2hs_hosts_lines);

    if (is_records_changed)
    {
        spdlog::info("Detected changes in DNS result. Updating hosts file...");
        // there are difference between the server's response and the hosts file content.
        // update the hosts file.
        std::vector<str> new_d2hs_hosts_records = {};
        for (const auto &record : server_records)
        {
            new_d2hs_hosts_records.push_back(record.to_hosts_line());
        }
        str new_resolve_content = hosts_file.write_d2hs_lines(new_d2hs_hosts_records, program_cfg.dry_run);
        if (program_cfg.dry_run)
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