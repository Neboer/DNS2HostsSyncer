#include "restapi.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>
#include <curl/curl.h>

namespace d2hs
{
    // Callback function to handle the response data
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s)
    {
        size_t totalSize = size * nmemb;
        s->append((char*)contents, totalSize);
        return totalSize;
    }

    json get_json_from_pdns_server(const rrpool_config &rrpool_cfg)
    {
        CURL* curl;
        CURLcode res;
        std::string readBuffer;

        curl = curl_easy_init();
        if(curl) {
            std::string url = fmt::format("{}/api/v1/servers/{}/zones/{}", rrpool_cfg.api_endpoint_url, rrpool_cfg.server_name, rrpool_cfg.zone_name);
            struct curl_slist* headers = nullptr;
            headers = curl_slist_append(headers, fmt::format("X-API-KEY: {}", rrpool_cfg.api_key).c_str());

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L); // 5 seconds timeout
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

            res = curl_easy_perform(curl);
            if(res == CURLE_OK) {
                long response_code;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
                if(response_code == 200) {
                    curl_easy_cleanup(curl);
                    curl_global_cleanup();
                    return json::parse(readBuffer);
                } else {
                    spdlog::error("Failed to get normal response from PowerDNS API. Status code: {}, Body: {}", response_code, readBuffer);
                    curl_easy_cleanup(curl);
                    curl_global_cleanup();
                    throw std::runtime_error("HTTP error");
                }
            } else {
                spdlog::error("A network error occurred: {}", curl_easy_strerror(res));
                curl_easy_cleanup(curl);
                curl_global_cleanup();
                throw std::runtime_error("network error");
            }
        } else {
            curl_global_cleanup();
            throw std::runtime_error("Failed to initialize CURL");
        }
    }
}