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

        void setup()
        {
          _log(logging::Info) << LOG_VIRAL << " successfully initialized" << std::endl;
        }

    private:
      logging::Logger _log{"spider-viral", logging::Level::Debug};
    };
}
#endif //SPIDER_CLIENT_VIRAL_HPP
