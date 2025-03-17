#include "hostsline.h"

namespace d2hs
{
    HostsLine::HostsLine(str hosts_line)
    {
        // Parse the hosts line
        size_t pos = hosts_line.find('#');
        if (pos != str::npos)
        {
            comment = hosts_line.substr(pos);
            hosts_line = hosts_line.substr(0, pos);
        }

        pos = hosts_line.find('\t');
        if (pos != str::npos)
        {
            name = hosts_line.substr(0, pos);
            value = hosts_line.substr(pos + 1);
        }
        else
        {
            pos = hosts_line.find(' ');
            if (pos != str::npos)
            {
                name = hosts_line.substr(0, pos);
                value = hosts_line.substr(pos + 1);
            }
            else
            {
                name = hosts_line;
                value = "";
            }
        }
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
}
