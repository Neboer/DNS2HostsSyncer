# 构建指南

本指南旨在提供不同平台的软件构建方法。DNS2HostsSyncer使用C++编写，配合CMake构建，主要依赖为curl,nlohmann-json,spdlog和argparse，在Windows中依赖vcpkg-MSVC工具链配合CMake进行构建，而在Linux下则利用GNU工具链配合CMake进行构建。本软件使用CPack在Linux中打deb包，使用独立的NSIS（不集成于CMake）在Windows中打exe安装包。本软件自带一个计划任务计时器，使用安装包安装本软件时，在Windows和Linux系统中会自动配置计划任务（Windows使用Task Sheduler、Linux使用systemd timer），可以定期执行一次以自动与上游服务器保持同步。本软件的MacOS软件仓库与软件打包由Cikaros单独负责，并不包含在本构建指南中，未来会考虑支持。

## Windows构建 

1. 需要准备vcpkg和MSVC开发环境。
2. 执行以下命令
    ```powershell
    cmake --preset windows-vcpkg -B build
    ```
    完成配置
3. 执行命令
    ```
    cmake --build build --config Release
    ```
    完成构建，此时构建的内容会写在 `build/Release` 下。
