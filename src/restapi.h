#pragma once
#include "rrpool_conf.h"
#include "record.h"
#include "vector"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace d2hs
{
    json get_json_from_pdns_server(const rrpool_config &cfg);
}