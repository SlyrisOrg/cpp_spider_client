//
// Created by doom on 03/11/17.
//

#ifndef SPIDER_CLIENT_OSXVIRAL_HPP
#define SPIDER_CLIENT_OSXVIRAL_HPP

#ifndef SPIDER_CLIENT_VIRAL_HPP
#error Do not include this file directly
#endif

namespace spi::details
{
    class ViralImpl
    {
    protected:
        bool antiPreload() noexcept
        {
            return true;
        }
    };
}

#endif //SPIDER_CLIENT_OSXVIRAL_HPP
