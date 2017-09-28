//
// Created by roman sztergbaum on 28/09/2017.
//

#ifndef SPIDER_CLIENT_LOGHANDLE_HPP
#define SPIDER_CLIENT_LOGHANDLE_HPP

#include <string_view>
#include "Configuration.hpp"

#define LOG_HANDLE utl::LightBlue << "{'LogHandle'}" << utl::Reset

namespace spi
{
    class LogHandle
    {
    public:
        ~LogHandle()
        {
            _log(lg::Info) << LOG_HANDLE << " shutting down." << std::endl;
        }
    public:
        void setup()
        {
            _log(lg::Info) << LOG_HANDLE << " log directory path -> ['" << _workingDir << "'].\n";
            _log(lg::Info) << LOG_HANDLE << " successfully initialized\n";
        }

    private:
        lg::Logger _log{"spider-log-handle", logging::Level::Debug};
        fs::path _workingDir{fs::temp_directory_path()};
    };
}

#endif //SPIDER_CLIENT_LOGHANDLE_HPP
