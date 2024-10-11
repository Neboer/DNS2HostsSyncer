#include "parse_args.h"
#include <spdlog/spdlog.h>

namespace d2hs
{
    program_arguments parse_args(int argc, char **argv)
    {
        argparse::ArgumentParser parser("d2hs", "2.3");

        parser.add_argument("-d", "--dry-run")
            .flag()
            .help("dry run");
        parser.add_argument("-f", "--hosts-file-path")
            .metavar("HOSTS_FILE_PATH")
            .help("Hosts file path, default is same as the config.");
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

        if (parser.present("--hosts-file-path")) {
            return {
                parser.get<std::string>("--config-file-location"),
                parser.get<bool>("--dry-run"),
                parser.get<std::string>("--hosts-file-path"),
                true
            };
        } else {
            return {
                parser.get<std::string>("--config-file-location"),
                parser.get<bool>("--dry-run"),
                "",
                false
            };
        }
    }
}
