#include "hosts_file.h"
#include <fstream>
#include <spdlog/spdlog.h>
#include <iostream>
#include <sstream>

namespace d2hs
{
    HostsFile::HostsFile(str file_path) : head_content(""), body_lines(), tail_content("")
    {
        hosts_file_path = file_path;
        std::ifstream hosts_file(hosts_file_path);

        if (!hosts_file.is_open())
        {
            spdlog::critical("Failed to open hosts file: {}", hosts_file_path);
            throw std::runtime_error("Failed to open hosts file.");
        }
        else
        {
            spdlog::info("Opened hosts file: {}", hosts_file_path);
        }

        int current_pos = 0; // 0: head, 2: body, 4: tail
        const int POS_HEAD = 0;
        const int POS_BODY = 2;
        const int POS_TAIL = 4;
        str line;
        int delimiter_count = 0;
        while (std::getline(hosts_file, line))
        {
            bool is_delimiter = false;
            if (line == HOSTS_DELIMITER)
            {
                is_delimiter = true;
                delimiter_count++;
            }

            if (current_pos == POS_HEAD)
            {
                if (is_delimiter)
                {
                    current_pos = POS_BODY;
                }
                else
                {
                    head_content += line + '\n';
                }
            }
            else if (current_pos == POS_BODY)
            {
                if (is_delimiter)
                {
                    current_pos = POS_TAIL;
                }
                else
                {
                    body_lines.push_back(HostsLine(line));
                }
            }
            else if (current_pos == POS_TAIL)
            {
                if (hosts_file.eof())
                {
                    // 如果hosts文件的最后一行缺少换行符，我们也不要手动添加。
                    tail_content += line;
                }
                else
                {
                    tail_content += line + '\n';
                }
            }
            else
            {
                continue;
            }
        }
        hosts_file.close();

        if (delimiter_count != 2)
        {
            spdlog::critical("Invalid hosts file: {}", hosts_file_path);
            throw std::runtime_error("Invalid hosts file, delimiter count is not 2.");
        }
    }

    std::vector<HostsLine> HostsFile::get_body_lines()
    {
        return body_lines;
    }

    void HostsFile::save_with_new_body(const std::vector<HostsLine> &new_body, const bool dry_run)
    {
        std::stringstream output_buffer;
        output_buffer << head_content << HOSTS_DELIMITER << '\n';
        for (const auto &line : new_body)
        {
            output_buffer << line.dump() << '\n';
        }
        output_buffer << HOSTS_DELIMITER << '\n'
                      << tail_content;

        if (!dry_run)
        {
            // update hosts file
            std::ofstream hosts_file(hosts_file_path);
            if (!hosts_file.is_open())
            {
#ifdef _WIN32
                char buf[512];
                strerror_s(buf, sizeof(buf), errno);
                spdlog::critical("Failed to write hosts file: {}, {}", hosts_file_path, buf);
#else
                spdlog::critical("Failed to write hosts file: {}, {}", hosts_file_path, strerror(errno));
#endif
                throw std::runtime_error("failed to write hosts file");
            }
            hosts_file << output_buffer.str();
            spdlog::info("Hosts file write completed.");
        }
    }

    const str get_os_default_hosts_file_path()
    {
#ifdef _WIN32
        return "C:\\Windows\\System32\\drivers\\etc\\hosts";
#elif defined(__APPLE__)
        return "/private/etc/hosts";
#else
        return "/etc/hosts";
#endif
    }
}
