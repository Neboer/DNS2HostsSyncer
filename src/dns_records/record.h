#pragma once
#include <string>

using str = std::string;

namespace d2hs
{
    enum RecordType
    {
        A = 1,
        AAAA = 2,
        CNAME = 5,
        OTHER = 0
    };

    struct RRRecord
    {
        str name;
        str value;
        RecordType type;
    };

    RecordType str_to_record_type(const str &record_type_str);
}
