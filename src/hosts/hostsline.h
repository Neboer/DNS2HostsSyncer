#pragma once
#include <string>
#include <vector>

using str = std::string;

namespace d2hs
{
    class HostsLine
    {
    public:
        HostsLine(str hosts_line);
        HostsLine(str name, str value, str comment);
        str name;
        str value;
        str comment;

        str dump() const;
        bool operator==(const HostsLine &other) const;
    };
}