project('d2hs', 'cpp', default_options: ['cpp_std=c++20'], version: '0.1.0')
nlohmann_json = dependency('nlohmann_json')
spdlog = dependency('spdlog')
argparse = dependency('argparse')
restclient = dependency('restclient-cpp')
executable(
    'd2hs',
    [
        'src/main.cpp',
        'src/parse_args.cpp',
        'src/protocol.cpp',
        'src/record.cpp',
        'src/restapi.cpp',
        'src/hosts_file.cpp',
        'src/parse_config.cpp',
        'src/rrpool_conf.cpp'
    ],
    dependencies: [nlohmann_json, spdlog, argparse, restclient],
    install: true
)