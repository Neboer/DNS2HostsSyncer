#pragma once
#include <string>
#include <vector>
#include "hostsline.h"

using str = std::string;

// TODO: don't hardcode this.
constexpr auto HOSTS_DELIMITER = "#d2hs_delimiter";

namespace d2hs
{
    // HostsFile 对象需要快速使用快速关闭，在HostsFile运行期间Hosts文件不应该被修改。
    // 在两个delimiters之间的内容是Hosts文件的主体内容，我们叫它body。
    // Hosts文件的头部和尾部是不可变的，我们叫它head和tail。
    class HostsFile
    {
    public:
        str hosts_file_path;
        HostsFile(str file_path);
        std::vector<HostsLine> get_body_lines();
        void save_with_new_body(const std::vector<HostsLine> &new_body, const bool dry_run = false);

    private:
        // File = head + delimiter\n + body + delimiter\n + tail
        str head_content;
        std::vector<HostsLine> body_lines;
        str tail_content;
    };

    const str get_os_default_hosts_file_path();
}
