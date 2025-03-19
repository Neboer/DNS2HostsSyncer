# DNS2HostsSyncer
用于将DNS记录同步到hosts文件的小型工具，可以将一个PowerDNS服务器中的解析记录同步到本地hosts文件中，可以快速方便的定期执行，不需要设置系统级DNS，加快域名解析速度。

## 项目概述
DNS到Hosts文件同步工具（d2hs）是一个设计用于从PowerDNS上游服务器同步DNS记录到本地hosts文件的工具。该工具通过PowerDNS Admin API获取指定区域的DNS记录，并据此更新hosts文件。同时具备监控hosts文件变更的功能，确保与上游服务器保持同步。

## 构建与安装
本项目使用CMake构建系统，需要以下依赖：
- [nlohmann_json](https://github.com/nlohmann/json)
- [spdlog](https://github.com/gabime/spdlog)
- [argparse](https://github.com/p-ranav/argparse)
- libcurl

详细构建指南请参阅[BUILD](./BUILD.md)文档。
在Windows、Debian系统中安装可以直接在Release中下载对应系统的安装包。
MacOS系统中的安装方法如下：
```sh
brew tap cikaros/tap
brew install d2hs
brew services start d2hs
```
注意MacOS的安装包由Cikaros专门维护，理论上本仓库目前并不包含在MacOS中相关的构建配置。

ArchLinux可以安装[dns2hostssyncer-git](https://aur.archlinux.org/pkgbase/dns2hostssyncer-git)这个AUR包。

## 配置说明
| 操作系统    | 默认配置文件路径                          |
|-------------|--------------------------------------------|
| Linux       | `/etc/d2hs/d2hs.json`                      |
| macOS       | `/etc/d2hs/d2hs.json`                      |
| Windows     | `C:\ProgramData\neboer\DNS2HostsSyncer`         |

配置文件采用JSON格式，包含以下配置项：

### `hosts_file_path`
- **类型:** 字符串
- **描述:** 指定需要更新的hosts文件路径
- **示例:** `"/etc/hosts"`

### `rrpools`
- **类型:** 对象数组
- **描述:** 资源记录池配置，定义连接PowerDNS API的参数集合
- **示例:**
  ```json
  "rrpools": [
      {
          "api_endpoint_url": "http://api.example.com:8081",
          "api_key": "secret_api_key",
          "server_name": "localhost",
          "zone_name": "test.server."
      }
  ]
  ```

#### 资源记录池配置项
每个资源记录池对象包含以下字段：

- **`api_endpoint_url`**
  - **类型:** 字符串
  - **描述:** PowerDNS API端点URL
  - **示例:** `"http://api.example.com:8081"`

- **`api_key`**
  - **类型:** 字符串
  - **描述:** PowerDNS API鉴权密钥
  - **示例:** `"secret_api_key"`

- **`server_name`**
  - **类型:** 字符串
  - **描述:** PowerDNS服务器名称
  - **示例:** `"localhost"`

- **`zone_name`**
  - **类型:** 字符串
  - **描述:** 需要同步的DNS区域名称
  - **示例:** `"test.server."`

## 使用指南
基础使用格式：
```bash
用法: d2hs [--help] [--version] [--dry-run] [--hosts-file-path HOSTS_FILE_PATH] [--config-file-location CONFIG_FILE_LOCATION]

可选参数:
  -h, --help                  显示帮助信息并退出
  -v, --version               显示版本信息并退出
  -d, --dry-run               试运行模式
  -f, --hosts-file-path       指定hosts文件路径 [默认系统hosts]
  -c, --config-file-location  指定配置文件路径 [参考配置说明]
  -l, --log-file-location     指定日志文件路径
```

在Windows上安装后，自动添加的计划任务会将软件产生的日志文件写入`C:\ProgramData\neboer\DNS2HostsSyncer\D2HSAutoSync.log`中。

## hosts文件路径
如果不传入hosts文件路径，则在不同的操作系统中，d2hs会在不同的路径下寻找默认的hosts文件。

| 操作系统    | 默认hosts文件路径                          |
|-------------|--------------------------------------------|
| Windows     | `C:\Windows\System32\drivers\etc\hosts`    |
| macOS       | `/private/etc/hosts`                       |
| Linux/Unix  | `/etc/hosts`                               |

## 功能特性
- **记录同步:** 从指定DNS区域获取记录并更新hosts文件
- **试运行模式:** 支持预演变更而不实际修改文件

## Hosts文件管理规范
程序按以下方式管理hosts文件：
```
# 静态主机名解析表
# 详见hosts(5)手册
# 10.0.0.1 some.records

#d2hs_delimiter
192.168.1.1 one.internal.network
192.168.1.2 two.internal.network
192.168.1.3 three.internal.network
192.168.1.4 four.internal.network
2001::1 ipv6.support
#d2hs_delimiter
::1 another.host
10.0.0.2 other.records
```
程序仅处理两个`#d2hs_delimiter`标记之间的内容。通过对比API返回记录与标记间内容进行同步，若标记不存在则会自动创建。记录顺序不影响对比结果（程序会进行排序比较）。

## 注意事项
- 确保API密钥具有访问目标DNS区域的权限
- 需要具备hosts文件的写权限（建议使用sudo运行）
- API基础URL请勿包含结尾斜杠`/`
- 使用本工具可帮助维护与DNS记录实时同步的hosts文件，提升资源管理效率

通过DNS2HostsSyncer，您可以轻松维护与指定DNS区域保持同步的hosts文件，实现高效的本地DNS资源管理。
