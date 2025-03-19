#include "converter.h"
#include <map>
#include <optional>
#include <spdlog/spdlog.h>

namespace d2hs
{
    HostsLine convert_A_AAAA_record_to_hosts_line(const RRRecord &record)
    {
        return HostsLine(record.name, record.value);
    }

    struct RecordsDictionary
    {
        std::map<str, str> address_lookup_table; // A AAAA
        std::map<str, str> cname_lookup_table;   // CNAME
    };

    // 提供递归解析CNAME记录的功能，直到解析到非CNAME记录为止。
    std::optional<str> resolve_cname_hostname(const str &hostname, RecordsDictionary &records_dict)
    {
        if (records_dict.cname_lookup_table.find(hostname) != records_dict.cname_lookup_table.end())
        {
            // 发现 CNAME 记录，继续递归解析。
            return resolve_cname_hostname(records_dict.cname_lookup_table[hostname], records_dict);
        }
        else if (records_dict.address_lookup_table.find(hostname) != records_dict.address_lookup_table.end())
        {
            // 发现 A 或 AAAA 记录，返回解析结果。
            return records_dict.address_lookup_table[hostname];
        }
        else
        {
            // 未发现记录，不能解析。
            std::nullopt;
        }
    }

    void insert_into_map_or_delete_key_if_exists(std::map<str, str> &map, const str &key, const str &value)
    {
        if (map.find(key) != map.end())
        {
            spdlog::warn("duplicate key found in map, deleting key: {}", key);
            map.erase(key);
        }
        map[key] = value;
    }

    std::vector<HostsLine> convert_records_to_hosts_lines(const std::vector<RRRecord> &records)
    {
        RecordsDictionary records_dict = RecordsDictionary();
        for (auto record : records)
        {
            switch (record.type)
            {
            case RecordType::A:
            case RecordType::AAAA:
                insert_into_map_or_delete_key_if_exists(records_dict.address_lookup_table, record.name, record.value);
                break;
            case RecordType::CNAME:
                // 检查此 CNAME 记录是否已经存在于 A 或 AAAA 记录中，如果存在则删除。
                if (records_dict.address_lookup_table.find(record.name) != records_dict.address_lookup_table.end())
                {
                    spdlog::warn("CNAME record {} found in A or AAAA records, deleting A or AAAA record", record.name);
                    records_dict.address_lookup_table.erase(record.name);
                }
                else
                {
                    insert_into_map_or_delete_key_if_exists(records_dict.cname_lookup_table, record.name, record.value);
                }
                break;
            default:
                spdlog::warn("unsupported record type: {}", record_type_to_str(record.type));
                break;
            }
        }

        std::vector<HostsLine> result_hosts_lines;
        // 先添加 A 和 AAAA 记录，再添加 CNAME 记录。
        for (auto addr_record : records_dict.address_lookup_table)
        {
            result_hosts_lines.push_back(HostsLine(addr_record.first, addr_record.second));
        }
        for (auto cname_record : records_dict.cname_lookup_table)
        {
            auto resolved_addr = resolve_cname_hostname(cname_record.first, records_dict);
            if (resolved_addr)
            {
                spdlog::info("resolved CNAME record: {} to {}", cname_record.first, *resolved_addr);
                result_hosts_lines.push_back(HostsLine(cname_record.first, *resolved_addr));
            }
            else
            {
                spdlog::warn("failed to resolve CNAME record: {}, skip it.", cname_record.first);
            }
        }
        return result_hosts_lines;
    }
}
