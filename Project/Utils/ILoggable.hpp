//
// Created by doom on 03/10/17.
//

#ifndef SPIDER_SERVER_ILOGGABLE_HPP
#define SPIDER_SERVER_ILOGGABLE_HPP

#include <Utils/Serialization.hpp>
#include <Utils/Stringification.hpp>
#include <Utils/JSONification.hpp>

namespace spi
{
    struct ILoggable : public Serializable, public IStringifiable, public IJSONifiable
    {
    };
}

#endif //SPIDER_SERVER_ILOGGABLE_HPP
