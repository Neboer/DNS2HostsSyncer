cmake_minimum_required(VERSION 4.00)

project(d2hs LANGUAGES CXX VERSION 4.0)
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
    src/configs/parse_args.cpp
    src/configs/parse_config.cpp
    src/configs/rrpool_conf.cpp
    src/dns_records/protocol.cpp
    src/dns_records/record.cpp
    src/dns_records/restapi.cpp
    src/hosts/hostsline.cpp
    src/hosts/hosts_file.cpp
    src/main.cpp
    src/converter.cpp
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

    # 安装配置文件到/etc/d2hs_config
    install(FILES config/example_d2hs.json
        DESTINATION /etc/d2hs
        RENAME d2hs.json
    )
    # Install systemd service and timer files
    install(FILES systemd/DNS2HostsSyncer.service systemd/DNS2HostsSyncer.timer
        DESTINATION /usr/lib/systemd/system
    )
    # 检测目标架构
    EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
    # 添加CPack配置
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_NAME "d2hs")
    set(CPACK_DEBIAN_FILE_NAME "d2hs_${PROJECT_VERSION}_${ARCHITECTURE}.deb")
    set(CPACK_DEBIAN_PACKAGE_VERSION ${PROJECT_VERSION})
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Neboer <rubinposter@gmail.com>")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "DNS to Hosts Synchronization Service")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libcurl4, libc6, libstdc++6, libgcc-s1")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/Neboer/DNS2HostsSyncer")
    set(CPACK_DEBIAN_PACKAGE_SECTION "net")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA 
        "${CMAKE_CURRENT_SOURCE_DIR}/package/debian/postinst;${CMAKE_CURRENT_SOURCE_DIR}/package/debian/prerm;${CMAKE_CURRENT_SOURCE_DIR}/package/debian/conffiles")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)

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
        message("create NSIS installer using build_windows_installer.ps1:")
        message("=============================\n")
    ]])

endif(WIN32)
include(CPack)
