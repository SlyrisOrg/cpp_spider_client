//
// Created by doom on 02/11/17.
//

#ifndef SPIDER_SERVER_JSONIFICATION_HPP
#define SPIDER_SERVER_JSONIFICATION_HPP

#include <string>

namespace spi
{
    struct JSON
    {
        static std::string quote(const std::string &str) noexcept
        {
            return "\"" + str + "\"";
        }
    };

    struct IJSONifiable
    {

        virtual std::string JSONify() const noexcept = 0;
    };
}

#endif //SPIDER_SERVER_JSONIFICATION_HPP
