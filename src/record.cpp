#include "record.h"
#include <stdexcept>
#include <format>
#include <spdlog/spdlog.h>

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
        else
        {
            spdlog::warn("Invalid record type {}.", record_type_str);
            throw std::invalid_argument("Invalid record type.");
        }
    }

    str RRRecord::to_hosts_line() const
    {
        return std::format("{} {}", value, name);
    }
}