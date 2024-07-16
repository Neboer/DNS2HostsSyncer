#pragma once
#include <string>

using str = std::string;

namespace d2hs
{
    enum RecordType
    {
        A = 1,
        AAAA = 2
    };

    struct RRRecord
    {
        str name;
        str value;
        RecordType type;

        str to_hosts_line() const;
        // bool operator==(RRRecord const& another_record);
    };

    RecordType str_to_record_type(const str &record_type_str);

}
