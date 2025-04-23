cmake_minimum_required(VERSION 3.17)

project(d2hs LANGUAGES CXX VERSION 4.1)
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
    set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Installation directory" FORCE)
endif()

if (UNIX)

    # 检测系统是否为Debian/Ubuntu并获取版本
    function(detect_debian_ubuntu_version OUTPUT_VAR)
        set(DETECTED_SYSTEM "Other")
        # 检查/etc/os-release文件是否存在
        if(EXISTS "/etc/os-release")
            # 读取os-release文件内容
            file(STRINGS "/etc/os-release" OS_RELEASE_CONTENT)
            # 解析ID和VERSION_ID
            set(ID "")
            set(VERSION_ID "")
            
            foreach(line IN LISTS OS_RELEASE_CONTENT)
                if(line MATCHES "^ID=(.*)")
                    set(ID "${CMAKE_MATCH_1}")
                    # 去除可能的引号
                    string(REGEX REPLACE "^[\"'](.*)[\"']$" "\\1" ID "${ID}")
                elseif(line MATCHES "^VERSION_ID=(.*)")
                    set(VERSION_ID "${CMAKE_MATCH_1}")
                    # 去除可能的引号
                    string(REGEX REPLACE "^[\"'](.*)[\"']$" "\\1" VERSION_ID "${VERSION_ID}")
                endif()
            endforeach()
            # 检查是否为Debian或Ubuntu
            if(ID STREQUAL "debian" OR ID STREQUAL "ubuntu")
                # 提取主版本号 (如从"24.04"中提取"24")
                string(REGEX MATCH "^[0-9]+" MAJOR_VERSION "${VERSION_ID}")
                if(NOT MAJOR_VERSION STREQUAL "")
                    # 格式化输出字符串
                    string(TOLOWER "${ID}" ID_LOWER)
                    string(SUBSTRING "${ID_LOWER}" 0 1 FIRST_CHAR)
                    string(SUBSTRING "${ID_LOWER}" 1 -1 REST_CHARS)
                    string(TOUPPER "${FIRST_CHAR}" FIRST_CHAR)
                    set(FORMATTED_ID "${FIRST_CHAR}${REST_CHARS}")
                    
                    set(DETECTED_SYSTEM "${FORMATTED_ID}${MAJOR_VERSION}")
                endif()
            endif()
        endif()
        set(${OUTPUT_VAR} "${DETECTED_SYSTEM}" PARENT_SCOPE)
    endfunction()

    detect_debian_ubuntu_version(OS_VERSION)
    message(STATUS "Detected OS version: ${OS_VERSION}")


    include(GNUInstallDirs)
    
    # Installation rules for executable
    install(TARGETS d2hs
        RUNTIME DESTINATION bin
    )

    # 安装示例配置文件到共享目录
    message(STATUS "CMAKE_INSTALL_SYSCONFDIR: ${CMAKE_INSTALL_FULL_SYSCONFDIR}/d2hs")
    install(FILES config/example_d2hs.json
        DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/d2hs
        RENAME d2hs.json
    )
    
    # Install systemd service and timer files
    install(FILES systemd/DNS2HostsSyncer.service systemd/DNS2HostsSyncer.timer
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/systemd/system
    )
    # 检测目标架构
    EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
    # 添加CPack配置
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_NAME "d2hs")
    set(CPACK_DEBIAN_FILE_NAME "d2hs_${PROJECT_VERSION}_${OS_VERSION}_${ARCHITECTURE}.deb")
    set(CPACK_DEBIAN_PACKAGE_VERSION ${PROJECT_VERSION})
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Neboer <rubinposter@gmail.com>")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "DNS to Hosts Synchronization Service")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
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
