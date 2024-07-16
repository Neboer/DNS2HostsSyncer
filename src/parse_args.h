#pragma once
#include <argparse/argparse.hpp>
#include <string>
#include "rrpool_conf.h"

using str = std::string;

namespace d2hs
{
    rrpool_config parse_args_as_rrpool_config(int argc, char **argv);
}
