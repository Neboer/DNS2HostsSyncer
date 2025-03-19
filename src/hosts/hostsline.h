#pragma once
#include <string>
#include <vector>

using str = std::string;

namespace d2hs
{
    // 注意，这不是一个通用的hosts文件解析器，我们只支持D2HS自己的hosts文件格式。
    // d2hs hosts文件的格式是：每行一个host，host的格式是：`addr hostname`
    // 中间用空格分隔，没有注释，且每个IP地址只对应一个hostname。
    class HostsLine
    {
    public:
        HostsLine(str hosts_line);
        HostsLine(str name, str value);
        str hostname;
        str addr;
        // str comment; // 我们目前不支持注释

        str dump() const;
        bool HostsLine::operator<(const HostsLine& other) const;
        bool HostsLine::operator==(const HostsLine& other) const;
    private:
        std::vector<str> split_str_white_space(const str &s);
    };
}
