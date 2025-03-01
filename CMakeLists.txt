cmake_minimum_required(VERSION 3.10)

project(d2hs LANGUAGES CXX VERSION 2.3)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(WIN32)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/package_install" CACHE PATH "" FORCE)
    mark_as_advanced(FORCE CMAKE_INSTALL_PREFIX)
    
    # 设置扁平化目录结构
    set(CMAKE_INSTALL_BINDIR ".")
    set(CMAKE_INSTALL_LIBDIR ".")
endif()

# Source files for the project
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

# Find required packages
find_package(nlohmann_json REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(argparse REQUIRED)
find_package(CURL REQUIRED)

# Create the executable target
add_executable(d2hs ${SOURCES})

# Link the libraries to the executable
target_link_libraries(d2hs
    PRIVATE
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    argparse::argparse
    CURL::libcurl
)

# 设置默认安装前缀为相对路径（仅在用户未指定时生效）
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND UNIX)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation directory" FORCE)
endif()

if (UNIX)
    # Installation rules for executable
    install(TARGETS d2hs
        RUNTIME DESTINATION bin
    )

    # 检查系统配置文件是否存在
    if(NOT EXISTS "/etc/d2hs/d2hs.json")
        # Install configuration file with conditional check
        install(FILES config/example_d2hs.json
            DESTINATION /etc/d2hs
            RENAME d2hs.json
        )
    endif()

    # Install systemd service and timer files
    install(FILES systemd/DNS2HostsSyncer.service systemd/DNS2HostsSyncer.timer
        DESTINATION /usr/lib/systemd/system
    )
endif (UNIX)

message(STATUS "BINARY_DIR: ${CMAKE_BINARY_DIR}")

# Windows install rules
if(WIN32)

    install(TARGETS d2hs
        RUNTIME DESTINATION . COMPONENT Runtime
        LIBRARY DESTINATION . COMPONENT Runtime
    )

    install(FILES 
        $<TARGET_RUNTIME_DLLS:d2hs>
        TYPE BIN
        COMPONENT Runtime
    )

    install(CODE [[
        message("\n\n")
        file(TO_NATIVE_PATH "${CMAKE_INSTALL_PREFIX}" NATIVE_INSTALL_DIR)
        message("=== installer files have been prepared ===")
        message("location: ${NATIVE_INSTALL_DIR}")
        message("create NSIS installer using command bellow:")
        message("makensis -DINSTDIR=${NATIVE_INSTALL_DIR} package/installer.nsi")
        message("=============================\n")
    ]])

endif(WIN32)
