#pragma once
#include <nlohmann/json.hpp>
#include <vector>
#include "rrpool_conf.h"

using json = nlohmann::json;

namespace d2hs
{
    struct program_config
    {
        std::vector<rrpool_config> rrpool_cfgs;
        str hosts_file_path;
    };

    program_config parse_config_file(const str &config_file_location);
    str get_default_config_file_location();
}
