# DNS2HostsSyncer
Small tools for syncing DNS Records with hosts file.

## Overview
The DNS to Hosts File Syncer (d2hs) is a tool designed to synchronize DNS records from a PowerDNS upstream server to your hosts file. It leverages the PowerDNS Admin API to fetch DNS records for a specific zone and updates the hosts file accordingly. Additionally, the tool monitors changes in the hosts file to ensure records remain in sync with the upstream server.

## Build and Installation
we use meson build system to build the program. It requires 4 deps:
- [nlohmann_json](https://github.com/nlohmann/json)
- [spdlog](https://github.com/gabime/spdlog)
- [argparse](https://github.com/p-ranav/argparse)
- [restclient](https://github.com/mrtazz/restclient-cpp)

To build and install the project on Linux, you need to install these dependencies first.

Build instructions on Arch Linux:
(need AUR Helper yay installed)
```bash
yay -S nlohmann-json spdlog-git argparse
# install restclient
git clone https://github.com/mrtazz/restclient-cpp.git
cd restclient-cpp
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
cmake --install build
# Currently, the AUR package restclient uses autotools build and does not provide enough package information for build systems like pkgconfig, CMake or meson. So we build it with CMake to solve this problem.
```
After that, install out package.
```bash
git clone https://github.com/Neboer/DNS2HostsSyncer
cd DNS2HostsSyncer
meson setup build
meson compile -C build
meson install -C build
```
Then you have the software `d2hs` installed.
```bash
d2hs --help
```

## Usage
The basic usage pattern for d2hs is as follows:

```bash
./builddir/d2hs --api-endpoint-url API_ENDPOINT_URL --api-key API_KEY --server-name SERVER_NAME --zone-name ZONE_NAME [options]
Command-Line Arguments
-h, --help: Shows the help message and exits.
-v, --version: Prints the version information and exits.
-u, --api-endpoint-url API_ENDPOINT_URL: Specifies the API endpoint URL (required).
-k, --api-key API_KEY: Specifies the API key for authenticating with the API endpoint (required).
-s, --server-name SERVER_NAME: Specifies the server name where the PowerDNS instance is running (required).
-z, --zone-name ZONE_NAME: Specifies the zone name to synchronize (required).
-d, --dry-run: Executes a dry run, which shows what changes would be made without actually modifying the hosts file.
-f, --hosts-file-path HOSTS_FILE_PATH: Specifies the path to the hosts file. If not provided, the default is "/etc/hosts".
```

Here is an example of how to use d2hs:

Synchronize DNS records from the PowerDNS upstream server:

```bash
d2hs -u http://test.dns.site:1234 -k test_key -s localhost -z test.domain. -f /home/test/temp/hosts
```

## Features
- Synchronization: Fetches DNS records for the specified zone and updates the hosts file accordingly.
- Dry Run: Allows you to preview the changes before actually modifying the hosts file.

## Hosts file
The program manage hosts file in a way like this:
```
# Static table lookup for hostnames.
# See hosts(5) for details.
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
D2fs will only focus on the lines between two delimiters `#d2hs_delimiter`. It will check if the returned records are the same as the lines between delimiters. If not, or if the d2hs_delimiter does not exist, d2hs will update its content or create delimiters in this file. The sequence of the lines has nothing to do with this check -- they are sorted before comparation.

## Notes
- Ensure that the API key provided has the necessary permissions to access the required zones on the PowerDNS server.
- The tool requires appropriate permissions to modify the hosts file. Run the tool with sufficient privileges.
- Ensure there is no trailing slash / in your base API url.

By using the DNSToHostsSyncer, you can maintain an up-to-date hosts file that reflects the DNS records of your specified zone, facilitating easier management and quicker access to your DNS resources.
