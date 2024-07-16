#pragma once
#include "rrpool_conf.h"
#include "record.h"
#include "vector"

namespace d2hs
{
    std::vector<Record> get_records_from_rr_pool(const rrpool_config &cfg);
}