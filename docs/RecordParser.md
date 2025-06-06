# Record Parser

解析器支持解析如下DNS解析记录：
A
AAAA
CNAME

D2HS暂时无法处理NS记录，同时解析器不负责解析外域域名——这不是我们的任务。DNS2HostsSyncer不是DNS服务器，Hosts文件的功能严重受限，我们希望成为一个稳定的内网域名同步器，而非越过DNS服务器处理外部域名。DNS2HostsSyncer应该可以配合DNS服务器使用，其不应该影响DNS服务器的解析结果，也不应该代替DNS服务器使用。

因此，D2HS在遇到CNAME解析的目标并不在已有的A/AAAA记录中时，会忽略此CNAME记录，不会向Hosts中添加。
同时，D2HS在遇到同一个域名存在多个A/AAAA记录的时候，不会添加任何记录，因为这与DNS服务器返回的结果是冲突的。
D2HS会像DNS递归查询一样递归的解析CNAME记录，直到解出地址或遇到外域而失败。
由于D2HS会去重，CNAME的解析发生在去重之后，因此CNAME解析到多解析的域名一样会留空。

由于Hosts File不支持通配符，因此含有通配符的记录一律不会被解析，当然我们会尽量遵守通配符记录。
Hosts File不支持多解析，虽然多个解析记录不会引起错误但只有第一个会生效。因此遇到一个域名对应多个DNS记录的情况，我们会报错。

实际解析器的解析顺序如下：
A+AAAA -> 去重 -> CNAME


下一个问题，D2HS如此操作，确实不会产生重复的解析记录吗？是否需要在CNAME之后再额外的经过一次合并？答案很简单：不会。

## 实际Converter的行为

Converter主要做如下的事情：
1. 扫描所有输入的RRRecords，提取其中不重复域名的A和AAAA记录，整理进map A_AAAA_lookup_table中。
2. 利用A_AAAA_lookup_table递归的解析所有CNAME域名到IP地址。
3. 将map A_AAAA_lookup_table转换成HostsLine列表，和解析到IP地址的所有CNAME一起做HostsLine列表输出。
