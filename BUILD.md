# 构建指南

本指南旨在提供不同平台的软件构建方法。DNS2HostsSyncer使用C++编写，配合CMake构建，主要依赖为curl,nlohmann-json,spdlog和argparse，在Windows中依赖vcpkg-MSVC工具链配合CMake进行构建，而在Linux下则利用GNU工具链配合CMake进行构建。本软件使用CPack在Linux中打deb包，使用独立的NSIS（不集成于CMake）在Windows中打exe安装包。本软件自带一个计划任务计时器，使用安装包安装本软件时，在Windows和Linux系统中会自动配置计划任务（Windows使用Task Sheduler、Linux使用systemd timer），可以定期执行一次以自动与上游服务器保持同步。本软件的MacOS软件仓库与软件打包由Cikaros单独负责，并不包含在本构建指南中，未来会考虑支持。

注意，以下所有构建打包都需要用户在项目根目录作为工作目录，除非在构建过程中切换。

## Windows 构建打包

1. 需要准备vcpkg和MSVC开发环境。如果需要打包，需要安装 nsis，并将 makensis 添加进PATH环境变量。
2. 执行以下命令进行configure
    ```powershell
    cmake --preset windows-vcpkg -B build
    ```
    完成配置
3. 执行以下命令进行构建
    ```powershell
    cmake --build build --config Release
    ```
    完成构建，此时构建的内容会写在 `build/Release` 下。
4. 执行以下命令生成安装包
    ```powershell
    ./build_windows_installer.ps1
    ```
    完成安装包生成，生成的安装包位于 `build/DNS2HostsSyncer_Installer.exe`
    双击安装包即可完成安装。
    安装此包的时候会自动添加计划任务，默认不带参数执行d2hs，可以进入任务计划程序调整传递给 d2hs.exe 的参数。计划任务的名字叫作 D2HSAutoSync

## Debian 构建打包

0. 安装依赖
    ```sh
    apt install cmake build-essential file
    apt install libspdlog-dev nlohmann-json3-dev libcurl4-openssl-dev libargparse-dev
    ```
    注意，一定要安装file，否则可能会导致打包失败。
    在某些操作系统（如debian bookworm）中，可能不存在 libargparse-dev 这个包，这时就需要[手动构建](https://github.com/p-ranav/argparse#building-installing-and-testing)了。
    不必担心，argparse是一个仅有头文件的C++库，只有在构建的时候才被需要，用户在运行时是不需要下载此依赖的。

1. 使用 cmake 进行 configure
    ```sh
    cmake --preset linux-native -B build
    ```
2. 进入 build 文件夹
    ```sh
    cd build
    ```
3. 启动构建
    ```sh
    make package -j $(nproc)
    ```
    这样，文件会被构建在 build/d2hs_xx_xx.deb 路径下。
    注意，安装此debian包的时候会自动处理systemd服务以及配置文件的问题，不需要担心。
