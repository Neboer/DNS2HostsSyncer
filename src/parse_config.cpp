#include "parse_config.h"
#include "rrpool_conf.h"
#include <fstream>
#include <spdlog/spdlog.h>

namespace d2hs
{
    program_config parse_config_file(const str &config_file_location)
    {
        json config_json;
        std::ifstream config_file(config_file_location);
        if (!config_file.is_open())
        {
            spdlog::critical("Failed to open config file {}.", config_file_location);
            throw std::runtime_error("Failed to open config file.");
        }

        config_file >> config_json;

        std::vector<rrpool_config> rrpool_cfgs;
        if (config_json.contains("rrpools") &&
            config_json["rrpools"].is_array() &&
            config_json.contains("hosts_file_path") &&
            config_json["hosts_file_path"].is_string())
        {
            for (auto &rrpool_cfg : config_json["rrpools"])
            {
                rrpool_cfgs.push_back(parse_rrpool_config(rrpool_cfg));
            }

            return {
                rrpool_cfgs,
                config_json["hosts_file_path"]
                };
        } else {
            spdlog::critical("Malformed config file. Missing rrpools or hosts_file_path or type is wrong.");
            throw std::runtime_error("config parse error");
        }
    }
}