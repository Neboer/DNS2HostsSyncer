#include "hosts_file.h"
#include <fstream>
#include <spdlog/spdlog.h>

namespace d2hs
{
    HostsFile::HostsFile() : delimiter_pos(), file_content()
    {
        std::ifstream hosts_file(HOSTS_FILE_PATH);

        if (!hosts_file.is_open())
        {
            spdlog::error("Failed to open hosts file: {}", HOSTS_FILE_PATH);
            throw std::runtime_error("Failed to open hosts file.");
        }

        str line;
        while (std::getline(hosts_file, line))
        {
            file_content.push_back(line);
        }
        hosts_file.close();

        // fist, find the first two delimiter positions.
        for (int i = 0; i < file_content.size(); i++)
        {
            if (file_content[i] == HOSTS_DELIMITER)
            {
                delimiter_pos.push_back(i);
                if (delimiter_pos.size() == 2)
                {
                    break;
                }
            }
        }

        if (delimiter_pos.size() == 1)
        {
            // only one delimiter found. This is an Malformed hosts file.
            spdlog::error("Malformed hosts file. Missing second delimiter.");
            throw std::runtime_error("Malformed hosts file. Missing second delimiter.");
        }
        else if (delimiter_pos.size() == 0)
        {
            // no delimiter found. This file is untouched by d2hs.
            spdlog::info("No delimiter found. This is a new file for d2hs.");

            // add the delimiter to the content.
            file_content.push_back(HOSTS_DELIMITER);
            file_content.push_back(HOSTS_DELIMITER);
            delimiter_pos.push_back(file_content.size());
            delimiter_pos.push_back(file_content.size() + 1);
        }
        else
        {
            // two delimiters found. This is a valid d2hs hosts file.
            spdlog::info("Found two delimiters in hosts file.");
            // extract the lines between the delimiters
        }
    }

    std::vector<str> HostsFile::read_d2hs_lines()
    {
        std::vector<str> extracted_content(file_content.begin() + delimiter_pos[0] + 1, file_content.begin() + delimiter_pos[1]);

        return extracted_content;
    }

    void HostsFile::write_d2hs_lines(const std::vector<str> &new_content)
    {
        // replace the content between the delimiters.
        file_content.erase(file_content.begin() + delimiter_pos[0] + 1, file_content.begin() + delimiter_pos[1]);
        file_content.insert(file_content.begin() + delimiter_pos[0] + 1, new_content.begin(), new_content.end());
        delimiter_pos[1] = delimiter_pos[0] + new_content.size() + 1;

        std::ofstream hosts_file(HOSTS_FILE_PATH);
        if (!hosts_file.is_open())
        {
            spdlog::error("Failed to open hosts file: {}", HOSTS_FILE_PATH);
            return;
        }

        for (const auto &line : file_content)
        {
            hosts_file << line << '\n';
        }
        spdlog::info("Hosts file write completed.");
    }
}