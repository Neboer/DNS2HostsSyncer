#pragma once
#include <argparse/argparse.hpp>
#include <string>
#include "rrpool_conf.h"

using str = std::string;

namespace d2hs
{
    struct program_config
    {
        rrpool_config rrpool_cfg;
        bool dry_run;
        str hosts_file_path;
    };

    program_config parse_args(int argc, char **argv);
}
