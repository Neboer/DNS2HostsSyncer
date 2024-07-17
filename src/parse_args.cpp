#include "parse_args.h"
#include <spdlog/spdlog.h>

namespace d2hs
{
    program_arguments parse_args(int argc, char **argv)
    {
        argparse::ArgumentParser parser("d2hs", "2.0");

        parser.add_argument("-d", "--dry-run")
            .flag()
            .help("dry run");
        parser.add_argument("-f", "--hosts-file-path")
            .default_value("/etc/hosts")
            .metavar("HOSTS_FILE_PATH")
            .help("Hosts file path");
        parser.add_argument("-c", "--config-file-location")
            .default_value("/etc/d2hs/d2hs.json")
            .metavar("CONFIG_FILE_LOCATION")
            .help("Configuration file location");

        try
        {
            parser.parse_args(argc, argv);
        }
        catch (const std::exception &err)
        {
            std::cerr << err.what() << std::endl;
            std::cerr << parser;
            exit(1);
        }

        return {
            parser.get<std::string>("--config-file-location"),
            parser.get<bool>("--dry-run"),
            parser.get<std::string>("--hosts-file-path")
        };
    }
}
