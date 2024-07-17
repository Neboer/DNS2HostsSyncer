#include "restapi.h"
#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>
#include <iostream>
#include <format>
#include <spdlog/spdlog.h>

namespace d2hs
{
    json get_json_from_pdns_server(const rrpool_config &rrpool_cfg)
    {
        // curl -v -H 'X-API-KEY: abc' http://dns.example.com:8081/api/v1/servers/localhost/zones/abc.def. -vvv

        RestClient::Connection *conn = new RestClient::Connection(rrpool_cfg.api_endpoint_url);
        conn->SetTimeout(5); // 5 seconds timeout.
        conn->AppendHeader("X-API-KEY", rrpool_cfg.api_key);

        RestClient::Response r = conn->get(std::format("/api/v1/servers/{}/zones/{}", rrpool_cfg.server_name, rrpool_cfg.zone_name));
        if (conn->GetInfo().lastRequest.curlCode == CURLE_OK)
        {
            if (r.code == 200)
            {
                return json::parse(r.body);
            }
            else
            {
                spdlog::error("Failed to get normal response from PowerDNS API. Status code: {}, Body: {}", r.code, r.body);
                throw std::runtime_error("HTTP error");
            }
        }
        else
        {
            spdlog::error("A network error occurred: {}", curl_easy_strerror((CURLcode)conn->GetInfo().lastRequest.curlCode));
            throw std::runtime_error("network error");
        }
    }
}