#include "parse_args.h"
#include <spdlog/spdlog.h>
#include "../hosts/hosts_file.h"
#include "parse_config.h"

namespace d2hs
{
    program_arguments parse_args(int argc, char **argv)
    {
        argparse::ArgumentParser parser("d2hs", "4.0");

        parser.add_argument("-d", "--dry-run")
            .flag()
            .help("dry run");
        parser.add_argument("-f", "--hosts-file-path")
            .default_value(get_os_default_hosts_file_path())
            .metavar("HOSTS_FILE_PATH")
            .help("Hosts file path, default is same as the config.");
        parser.add_argument("-c", "--config-file-location")
            .default_value(get_default_config_file_location())
            .metavar("CONFIG_FILE_LOCATION")
            .help("Configuration file location");
        parser.add_argument("-l", "--log-file-location")
            .default_value("")
            .metavar("LOG_FILE_LOCATION")
            .help("log to specified file as well as stdout");
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
            parser.is_used("--log-file-location"),
            parser.get("--log-file-location"),

            parser.get<bool>("--dry-run"),
            parser.get<std::string>("--hosts-file-path"),
            true
        };
    }
}
