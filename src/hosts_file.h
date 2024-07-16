#pragma once
#include <string>
#include <vector>
#include "record.h"

using str = std::string;

constexpr auto HOSTS_FILE_PATH = "/etc/hosts";
constexpr auto HOSTS_DELIMITER = "#d2hs_delimiter";

namespace d2hs
{
    str record_to_host_line(const Record &record);
    std::vector<str> read_d2hs_hosts_content();
    void write_d2hs_hosts_content(const std::vector<str> &content);
    void compare_hosts_content(const std::vector<str> &content1, const std::vector<str> &content2);
}