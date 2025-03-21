#pragma once
#include <string>
#include <map>

using str = std::string;

namespace d2hs
{
    enum RecordType
    {
        A = 1,
        AAAA = 2,
        CNAME = 5,
        SOA = 100,
        OTHER = 0
    };

    static const std::map<str, RecordType> str_to_record_type_map = {
        {"A", RecordType::A},
        {"AAAA", RecordType::AAAA},
        {"CNAME", RecordType::CNAME},
        {"SOA", RecordType::SOA}
    };

    static const std::map<RecordType, str> record_type_to_str_map = {
        {RecordType::A, "A"},
        {RecordType::AAAA, "AAAA"},
        {RecordType::CNAME, "CNAME"},
        {RecordType::SOA, "SOA"}
    };

    struct RRRecord
    {
        str name;
        str value;
        RecordType type;
    };

    RecordType str_to_record_type(const str &record_type_str);
    str record_type_to_str(RecordType record_type);
}
