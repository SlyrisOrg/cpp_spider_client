//
// Created by doom on 03/10/17.
//

#ifndef SPIDER_SERVER_ILOGGABLE_HPP
#define SPIDER_SERVER_ILOGGABLE_HPP

#include <Utils/Serialization.hpp>
#include <Utils/Stringification.hpp>

namespace spi
{
    struct ILoggable : public ISerializable, public IStringifiable
    {
    };
}

#endif //SPIDER_SERVER_ILOGGABLE_HPP
