#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include "record.h"

using json = nlohmann::json;
using str = std::string;

namespace d2hs
{
    std::vector<RRRecord> parse_pdns_response(const json &response);
}