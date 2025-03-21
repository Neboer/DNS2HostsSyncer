#include "hostsline.h"
#include <sstream> // For string stream operations
#include <iterator>

namespace d2hs
{
    // Constructor to parse a hosts line into name, value, and comment
    HostsLine::HostsLine(str hosts_line)
    {
        auto parts = split_str_white_space(hosts_line);
        if (parts.size() >= 2)
        {
            addr = parts[0];
            hostname = parts[1];
        }
        else
        {
            // d2hs 使用的内部hosts行格式不正确，抛出异常，顶层应该报错并退出
            throw std::invalid_argument("Invalid hosts line: " + hosts_line);
        }
    }

    // Constructor to initialize with name, value, and comment
    HostsLine::HostsLine(str name, str value) : hostname(std::move(name)), addr(std::move(value))
    {
    }

    // Dump the HostsLine as a formatted string
    str HostsLine::dump() const
    {
        return addr + '\t' + hostname;
    }

    std::vector<str> HostsLine::split_str_white_space(const str &s)
    {
        std::istringstream iss(s);
        return {std::istream_iterator<str>{iss}, std::istream_iterator<str>{}}; // Split the string by whitespace
    }

    // Comparison operator for sorting
    bool HostsLine::operator<(const HostsLine &other) const
    {
        return addr < other.addr || (addr == other.addr && hostname < other.hostname);
    }

    // Equality operator for comparison
    bool HostsLine::operator==(const HostsLine &other) const
    {
        return addr == other.addr && hostname == other.hostname;
    }
}
