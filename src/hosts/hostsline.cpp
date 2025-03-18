#include "hostsline.h"
#include <sstream> // For string stream operations

namespace d2hs
{
    // Constructor to parse a hosts line into name, value, and comment
    HostsLine::HostsLine(str hosts_line)
    {
        std::istringstream stream(hosts_line);
        stream >> name; // Extract the name (first word)

        if (stream >> value) // Extract the value (second word, if present)
        {
            std::getline(stream, comment); // Extract the rest as a comment
            if (!comment.empty() && comment[0] == '#') // Remove leading '#' from the comment
            {
                comment = comment.substr(1);
            }
        }
    }

    // Constructor to initialize with name, value, and comment
    HostsLine::HostsLine(str name, str value, str comment)
        : name(std::move(name)), value(std::move(value)), comment(std::move(comment))
    {
    }

    // Dump the HostsLine as a formatted string
    str HostsLine::dump() const
    {
        str result = name;
        if (!value.empty())
        {
            result += '\t' + value;
        }
        if (!comment.empty())
        {
            result += " #" + comment;
        }
        return result;
    }

    // Comparison operator for sorting
    bool HostsLine::operator<(const HostsLine& other) const
    {
        return (name < other.name) || (name == other.name && value < other.value);
    }

    // Equality operator for comparison
    bool HostsLine::operator==(const HostsLine& other) const
    {
        return name == other.name && value == other.value && comment == other.comment;
    }
}
