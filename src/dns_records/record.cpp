#include "./record.h"
#include <stdexcept>

namespace d2hs
{
    RecordType str_to_record_type(const str &record_type_str)
    {
        try
        {
            return str_to_record_type_map.at(record_type_str);
        }
        catch (const std::out_of_range&)
        {
            return RecordType::OTHER;
        }
    }

    str record_type_to_str(RecordType record_type)
    {
        try
        {
            return record_type_to_str_map.at(record_type);
        }
        catch (const std::out_of_range&)
        {
            return "OTHER";
        }
    }
}
