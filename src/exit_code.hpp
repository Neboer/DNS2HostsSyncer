#pragma once
namespace d2hs
{
    enum ExitCode
    {
        SUCCESS = 0,
        LOG_INIT_FAILURE = 1,
        API_FAILURE = 2,
        HOSTS_UPDATE_FAILURE = 3,
        OTHER_FAILURE = 4
    };
}
