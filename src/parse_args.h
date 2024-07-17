#pragma once
#include <argparse/argparse.hpp>
#include <string>
#include "rrpool_conf.h"

using str = std::string;

namespace d2hs
{
    struct program_arguments
    {
        str config_file_location;
        bool dry_run;
        str hosts_file_path;
    };

    program_arguments parse_args(int argc, char **argv);
}
