#include "parse_args.h"
#include <spdlog/spdlog.h>

namespace d2hs
{
    rrpool_config parse_args_as_rrpool_config(int argc, char **argv)
    {
        argparse::ArgumentParser parser("D2HS");
        parser.add_argument("-u", "--api-endpoint-url")
            .help("API endpoint URL");
        parser.add_argument("-k", "--api-key")
            .help("API key");
        parser.add_argument("-s", "--server-name")
            .help("Server name");
        parser.add_argument("-z", "--zone-name").help("Zone name");

        try
        {
            parser.parse_args(argc, argv);
        }
        catch (const std::runtime_error &err)
        {
            spdlog::error(err.what());
            parser.print_help();
            exit(1);
        }

        return {
            parser.get<std::string>("--api-endpoint-url"),
            parser.get<std::string>("--api-key"),
            parser.get<std::string>("--server-name"),
            parser.get<std::string>("--zone-name")};
    }
}
