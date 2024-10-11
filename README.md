# DNS2HostsSyncer
Small tools for syncing DNS Records with hosts file.

## Overview
The DNS to Hosts File Syncer (d2hs) is a tool designed to synchronize DNS records from a PowerDNS upstream server to your hosts file. It leverages the PowerDNS Admin API to fetch DNS records for a specific zone and updates the hosts file accordingly. Additionally, the tool monitors changes in the hosts file to ensure records remain in sync with the upstream server.

## Build and Installation
we use meson/cmake build system to build the program. It requires 4 deps:
- [nlohmann_json](https://github.com/nlohmann/json)
- [spdlog](https://github.com/gabime/spdlog)
- [argparse](https://github.com/p-ranav/argparse)
- libcurl

If you use cmake install, the project will download and build deps automatically, except for libcurl.

You need to install all deps first only if you want to use cmake to build d2hs without downloading any package, just use your local package.

You always need to install all deps first if you use meson build. 

```bash
mkdir build
cd build
cmake ..
make
make install
```

Build instructions on Arch Linux:
(need AUR Helper yay installed)
```bash
yay -S nlohmann-json spdlog-git argparse curl
```
After that, build and install the program.

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

If you are using nix or nixos with flakes enabled, just run
```
nix build
```
to build. The built artifact lies in ./result/bin/d2hs.

## Configuration
The default configuration file is located at /etc/d2hs/d2hs.json.

The configuration file for `d2hs` is a JSON file that specifies the settings and parameters for synchronizing DNS records with the hosts file.

### `hosts_file_path`
- **Type:** String
- **Description:** Specifies the path to the hosts file that will be updated by `d2hs`.
- **Example:** `"/etc/hosts"`

### `rrpools`
- **Type:** Array of objects
- **Description:** Contains a list of resource record pools. Each pool defines a set of parameters to connect to a PowerDNS API and fetch DNS records.
- **Example:**
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

#### Inside `rrpools`
Each object inside the `rrpools` array should have the following fields:

- **`api_endpoint_url`**
  - **Type:** String
  - **Description:** The URL of the PowerDNS API endpoint.
  - **Example:** `"http://api.example.com:8081"`

- **`api_key`**
  - **Type:** String
  - **Description:** The API key used for authenticating with the PowerDNS API.
  - **Example:** `"secret_api_key"`

- **`server_name`**
  - **Type:** String
  - **Description:** The name of the PowerDNS server.
  - **Example:** `"localhost"`

- **`zone_name`**
  - **Type:** String
  - **Description:** The name of the DNS zone to synchronize.
  - **Example:** `"test.server."`


## Usage
The basic usage pattern for d2hs is as follows:

```bash
Usage: d2hs [--help] [--version] [--dry-run] [--hosts-file-path HOSTS_FILE_PATH] [--config-file-location CONFIG_FILE_LOCATION]

Optional arguments:
  -h, --help                  shows help message and exits 
  -v, --version               prints version information and exits 
  -d, --dry-run               dry run 
  -f, --hosts-file-path       Hosts file path [default: "/etc/hosts"]
  -c, --config-file-location  Configuration file location [default: "/etc/d2hs/d2hs.json"]
```

Here is an example of how to use d2hs:

Synchronize DNS records from the PowerDNS upstream server:

```bash
d2hs -c /etc/d2fs.json
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
D2fs will only focus on the lines between two delimiters `#d2hs_delimiter`. It will check if the returned records are the same as the lines between delimiters. If not, or if the d2hs_delimiter does not exist, d2hs will update its content or create delimiters in this file. The sequence of the lines has nothing to do with this check -- they are sorted before comparison.

## Notes
- Ensure that the API key provided has the necessary permissions to access the required zones on the PowerDNS server.
- The tool requires appropriate permissions to modify the hosts file. Run the tool with sufficient privileges.
- Ensure there is no trailing slash / in your base API url.

By using the DNSToHostsSyncer, you can maintain an up-to-date hosts file that reflects the DNS records of your specified zone, facilitating easier management and quicker access to your DNS resources.
