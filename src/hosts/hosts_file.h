#pragma once
#include <string>
#include <vector>

using str = std::string;

// TODO: don't hardcode this.
constexpr auto HOSTS_DELIMITER = "#d2hs_delimiter";

namespace d2hs
{
    class HostsFile
    {
    public:
        str hosts_file_path;
        HostsFile(str file_path);
        std::vector<str> read_d2hs_lines();
        str write_d2hs_lines(const std::vector<str> &new_content, bool dry_run);
        static str get_hosts_file_path();

    private:
        std::vector<int> delimiter_pos;
        std::vector<str> file_content;
    };
}