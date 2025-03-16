#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using str = std::string;

namespace d2hs
{
    struct rrpool_config
    {
        str api_endpoint_url; // http://[::1]:8566/
        str api_key;          // 123456

        str server_name; // localhost
        str zone_name;   // example.com
    };

    rrpool_config parse_rrpool_config(const json &rrpool_cfg);
}