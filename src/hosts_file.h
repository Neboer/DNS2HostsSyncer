#pragma once
#include <string>
#include <vector>
#include "record.h"

using str = std::string;

constexpr auto HOSTS_FILE_PATH = "/etc/hosts";
constexpr auto HOSTS_DELIMITER = "#d2hs_delimiter";

namespace d2hs
{
    class HostsFile
    {
    public:
        HostsFile();
        std::vector<str> read_d2hs_lines();
        void write_d2hs_lines(const std::vector<str> &new_content);

    private:
        std::vector<int> delimiter_pos;
        std::vector<str> file_content;
    };
}