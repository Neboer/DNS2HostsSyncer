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

    struct Record
    {
        str name;
        str value;
        RecordType type;
    };

}
