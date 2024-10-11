cmake_minimum_required(VERSION 3.5)

project(d2hs LANGUAGES CXX VERSION 2.2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES
    src/main.cpp
    src/parse_args.cpp
    src/protocol.cpp
    src/record.cpp
    src/restapi.cpp
    src/hosts_file.cpp
    src/parse_config.cpp
    src/rrpool_conf.cpp
)

find_package(nlohmann_json QUIET)
find_package(spdlog QUIET)
find_package(argparse QUIET)
find_package(CURL REQUIRED)

if(NOT (nlohmann_json_FOUND AND spdlog_FOUND AND argparse_FOUND))
    message(STATUS "Some packages not found, fetching required dependencies using FetchContent...")

    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3 # Specify the version you want
    )

    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.14.1 # Specify the version you want
    )

    FetchContent_Declare(
        argparse
        GIT_REPOSITORY https://github.com/p-ranav/argparse.git
        GIT_TAG v3.1 # Specify the version you want
    )

    FetchContent_MakeAvailable(nlohmann_json spdlog argparse)

endif()

add_executable(d2hs ${SOURCES})

target_link_libraries(d2hs
    PRIVATE
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    argparse::argparse
    CURL::libcurl
)

# Installation rules
install(TARGETS d2hs
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)

# Install configuration file and rename it to d2hs.json
install(FILES config/example_d2hs.json
    DESTINATION /etc/d2hs
    RENAME d2hs.json
)

# Install systemd service and timer files
install(FILES systemd/DNS2HostsSyncer.service systemd/DNS2HostsSyncer.timer
    DESTINATION /usr/lib/systemd/system
)
