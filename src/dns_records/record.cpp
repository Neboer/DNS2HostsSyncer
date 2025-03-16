#include "./record.h"

namespace d2hs
{
    RecordType str_to_record_type(const str &record_type_str)
    {
        if (record_type_str == "A")
        {
            return RecordType::A;
        }
        else if (record_type_str == "AAAA")
        {
            return RecordType::AAAA;
        }
        else if (record_type_str == "CNAME")
        {
            return RecordType::CNAME;
        }
        else
        {
            return RecordType::OTHER;
        }
    }
}