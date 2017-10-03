//
// Created by doom on 03/10/17.
//

#ifndef SPIDER_SERVER_STRINGIFICATION_HPP
#define SPIDER_SERVER_STRINGIFICATION_HPP

#include <string>

namespace spi
{
    struct IStringifiable
    {
    public:
        virtual std::string stringify() const noexcept = 0;
    };
}

#endif //SPIDER_SERVER_STRINGIFICATION_HPP
