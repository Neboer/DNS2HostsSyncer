#include "protocol.h"
#include "record.h"
#include <spdlog/spdlog.h>

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

namespace d2hs
{
    std::vector<RRRecord> parse_pdns_response(const json &response)
    {
        std::vector<RRRecord> output_records;

        for (const auto &rr_record : response["rrsets"])
        {
            const RecordType rt = str_to_record_type(rr_record["type"]);
            str dot_free_value = rr_record["records"][0]["content"];
            if (rt == RecordType::CNAME)
            {
                // CNAME记录的content是一个FQDN，我们需要移除末尾的“.”。
                dot_free_value = remove_fqdn(dot_free_value);
            }

            try
            {
                output_records.push_back(
                    {
                        remove_fqdn(rr_record["name"]),
                        dot_free_value,
                        rt    
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
