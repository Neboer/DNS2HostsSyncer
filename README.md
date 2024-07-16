# DNS2HostsSyncer
Small tools for syncing DNS Records with hosts file.

## Overview
The DNS to Hosts File Syncer (d2hs) is a tool designed to synchronize DNS records from a PowerDNS upstream server to your hosts file. It leverages the PowerDNS Admin API to fetch DNS records for a specific zone and updates the hosts file accordingly. Additionally, the tool monitors changes in the hosts file to ensure records remain in sync with the upstream server.

## Installation
we use meson build system to build the program. It requires 4 deps: nlohmann_json, spdlog, argparse and restclient.

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

## Notes
- Ensure that the API key provided has the necessary permissions to access the required zones on the PowerDNS server.
- The tool requires appropriate permissions to modify the hosts file. Run the tool with sufficient privileges.
- Ensure there is no trailing slash / in your base API url.

By using the DNSToHostsSyncer, you can maintain an up-to-date hosts file that reflects the DNS records of your specified zone, facilitating easier management and quicker access to your DNS resources.






