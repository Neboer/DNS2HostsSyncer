#include "protocol.h"
#include "record.h"
#include <spdlog/spdlog.h>

namespace d2hs
{
    str remove_fqdn(const str &domain_with_fqdn_dot)
    {
        if (domain_with_fqdn_dot.back() == '.')
        {
            return domain_with_fqdn_dot.substr(0, domain_with_fqdn_dot.size() - 1);
        }
        else
        {
            return domain_with_fqdn_dot;
        }
    }

    std::vector<RRRecord> parse_pdns_response(const json &response)
    {
        std::vector<RRRecord> output_records;

        for (const auto &rr_record : response["rrsets"])
        {
            try
            {
                output_records.push_back(
                    {
                        remove_fqdn(rr_record["name"]),
                        rr_record["records"][0]["content"],
                        str_to_record_type(rr_record["type"])    
                    });
            }
            catch (const std::invalid_argument &e)
            {
                // skip this record.
                spdlog::warn("Skipping record: {}", rr_record["name"].dump());
            }
        }

        return output_records;
    }
}
