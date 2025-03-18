#include "hostsline.h"

namespace d2hs
{
    HostsLine::HostsLine(str hosts_line)
    {
        
    }

    HostsLine::HostsLine(str name, str value, str comment="")
        : name(name), value(value), comment(comment)
    {
    }

    str HostsLine::dump() const
    {
        str result = name;
        if (!value.empty())
        {
            result += '\t' + value;
        }
        if (!comment.empty())
        {
            result += ' ' + comment;
        }
        return result;
    }

    bool HostsLine::operator<(const HostsLine& other)
    {
        return (name < other.name) || (name == other.name && value < other.value);
    }

    bool HostsLine::operator==(const HostsLine& other)
    {
        return name == other.name && value == other.value;
    }
}
