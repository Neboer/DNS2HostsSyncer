// Converter可以将一系列RRRecords转换成一系列HostLines列表，用来将远程DNS的记录转换成本地记录。
#pragma once
#include "hosts/hostsline.h"
#include "dns_records/record.h"

namespace d2hs
{
    std::vector<HostsLine> convert_records_to_hosts_lines(const std::vector<RRRecord> &records);
}
