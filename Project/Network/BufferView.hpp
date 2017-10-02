//
// Created by doom on 28/09/17.
//

#ifndef SPIDER_SERVER_BUFFERVIEW_HPP
#define SPIDER_SERVER_BUFFERVIEW_HPP

#include <cstddef>

namespace spi::net
{
    class BufferView
    {
    public:
        BufferView(void *data, size_t size) noexcept : _data(data), _size(size)
        {
        }

        void *data() const noexcept
        {
            return _data;
        }

        size_t size() const noexcept
        {
            return _size;
        }

    private:
        void *_data;
        size_t _size;
    };
}

#endif //SPIDER_SERVER_BUFFERVIEW_HPP
