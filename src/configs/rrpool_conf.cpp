#include "rrpool_conf.h"
#include <spdlog/spdlog.h>

namespace d2hs
{
    rrpool_config parse_rrpool_config(const json &rrpool_cfg)
    {
        if (rrpool_cfg.contains("api_endpoint_url") &&
            rrpool_cfg["api_endpoint_url"].is_string() &&
            rrpool_cfg.contains("api_key") &&
            rrpool_cfg["api_key"].is_string() &&
            rrpool_cfg.contains("server_name") &&
            rrpool_cfg["server_name"].is_string() &&
            rrpool_cfg.contains("zone_name") &&
            rrpool_cfg["zone_name"].is_string())
        {
            return {
                rrpool_cfg["api_endpoint_url"],
                rrpool_cfg["api_key"],
                rrpool_cfg["server_name"],
                rrpool_cfg["zone_name"]};
        }
        else
        {
            spdlog::critical("Malformed rrpool config {}.", rrpool_cfg.dump());
            throw std::runtime_error("malformed rrpool config");
        }
    }
}
