#include "protocol.h"
#include "record.h"

namespace d2hs
{
    str remove_fqdn(const str &domain_with_fqdn_dot)
    {
        return domain_with_fqdn_dot.substr(0, domain_with_fqdn_dot.find('.'));
    }

    std::vector<RRRecord> parse_pdns_response(const json &response)
    {
        std::vector<RRRecord> output_records;
        for (const auto &rr_record : response['rrsets'])
        {
            output_records.push_back(
                {rr_record['name'],
                 rr_record['records'][0]['content'],
                 str_to_record_type(rr_record['type'])});
        }
        return output_records;
    }
}
