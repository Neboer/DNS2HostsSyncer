#include "parse_args.h"
#include "restapi.h"
#include "protocol.h"
#include "rrpool_conf.h"
#include "record.h"
#include "hosts_file.h"
#include <algorithm>
#include <spdlog/spdlog.h>

int main(int argc, char **argv)
{
    // find host record in hosts file.
    d2hs::HostsFile hosts_file = d2hs::HostsFile();
    std::vector<str> d2hs_hosts_lines = hosts_file.read_d2hs_lines();

    auto current_rrpool_config = d2hs::parse_args_as_rrpool_config(argc, argv);

    spdlog::info("sending API request...");
    json record_response = d2hs::get_json_from_pdns_server(current_rrpool_config);
    spdlog::info("API request successful.");
    std::vector<d2hs::RRRecord> server_records = d2hs::parse_pdns_response(record_response);

    // for each record, check if it exists in the hosts file's d2fs section.
    auto found = std::find_if(server_records.begin(), server_records.end(), [&](const d2hs::RRRecord &record)
                              { 
                                // does it exist in the hosts file?
                                return std::find(d2hs_hosts_lines.begin(), d2hs_hosts_lines.end(), record.to_hosts_line()) != d2hs_hosts_lines.end(); });

    if (found != server_records.end())
    {
        spdlog::info("Detected changes in DNS result. Updating hosts file...");
        // there are difference between the server's response and the hosts file content.
        // update the hosts file.
        std::vector<str> new_d2hs_hosts_records = {};
        for (const auto &record : server_records)
        {
            new_d2hs_hosts_records.push_back(record.to_hosts_line());
        }

        hosts_file.write_d2hs_lines(new_d2hs_hosts_records);
        spdlog::info("Hosts file updated.");
    }
    else
    {
        spdlog::info("No changes detected in DNS result.");
    }

    return 0;
}