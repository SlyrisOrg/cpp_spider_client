//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_LINUXKEYLOGGER_HPP
#define SPIDER_CLIENT_LINUXKEYLOGGER_HPP

#include <memory>
#include <utils/Config.hpp>
#include <Network/IOManager.hpp>
#include <KeyLogger/KeyLogger.hpp>


#define KEYLOGGER_LOG "{'KeyLogger'}"

namespace spi
{
    class LinuxKeyLogger : public KeyLogger
    {
    public:
        explicit LinuxKeyLogger(net::IOManager &service) : _service(service)
        {
          // parse /proc/bus/input/devices
          // and construct the vectors of files to watch
        }

        ~LinuxKeyLogger() override
        {
          _log(logging::Info) << KEYLOGGER_LOG << "shutting down." << std::endl;
        }

        void setup() override
        {
          _log(logging::Info) << KEYLOGGER_LOG << "successfully initialized." << std::endl;
        }

        void run() override
        {
          _log(logging::Info) << KEYLOGGER_LOG << "started." << std::endl;
          // add the watchers on the files
          // non blocking read with callback on event
        }

        void stop() override
        {
          _log(logging::Info) << KEYLOGGER_LOG << "stopped." << std::endl;
          // remove the watchers
        }

    private:
        // only works with tek pc, need to be construte out of
        // the parsing of /proc/bus/input/devices
        net::IOManager &_service;
        std::vector<std::string> _keyPadToWatch{"/dev/input/event3"};
        std::vector<std::string> _mouseToWatch{"/dev/input/event4", "/dev/input/event7"};
    };
}

namespace spi::details
{
    static always_inline KeyLogPtr createKeyLogger(net::IOManager &service)
    {
      return std::make_unique<LinuxKeyLogger>(service);
    }
}
#endif //SPIDER_CLIENT_LINUXKEYLOGGER_HPP
