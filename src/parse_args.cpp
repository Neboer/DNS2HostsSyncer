#include "parse_args.h"
#include <spdlog/spdlog.h>

namespace d2hs
{

    program_config parse_args(int argc, char **argv)
    {
        argparse::ArgumentParser parser("d2hs", "0.1");
        parser.add_argument("-u", "--api-endpoint-url")
            .required()
            .metavar("API_ENDPOINT_URL")
            .help("API endpoint URL");
        parser.add_argument("-k", "--api-key")
            .required()
            .metavar("API_KEY")
            .help("API key");
        parser.add_argument("-s", "--server-name")
            .required()
            .metavar("SERVER_NAME")
            .help("Server name");
        parser.add_argument("-z", "--zone-name")
            .required()
            .metavar("ZONE_NAME")
            .help("Zone name");
        parser.add_argument("-d", "--dry-run")
            .flag()
            .help("dry run");
        parser.add_argument("-f", "--hosts-file-path")
            .default_value("/etc/hosts")
            .metavar("HOSTS_FILE_PATH")
            .help("Hosts file path");

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

        return program_config{
            {
                parser.get<str>("--api-endpoint-url"),
                parser.get<str>("--api-key"),
                parser.get<str>("--server-name"),
                parser.get<str>("--zone-name")
            },
            parser.get<bool>("--dry-run"),
            parser.get<str>("--hosts-file-path")
        };
    }
}
