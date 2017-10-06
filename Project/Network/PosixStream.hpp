//
// Created by roman sztergbaum on 05/10/2017.
//

#ifndef SPIDER_SERVER_ASYNCINTERACTIVE_HPP
#define SPIDER_SERVER_ASYNCINTERACTIVE_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/streambuf.hpp>
#include <Network/IOManager.hpp>


namespace spi::net
{
    class PosixStream
    {
    public:
        PosixStream(IOManager &mgr, int fd) : _input(mgr.get(), fd)
        {
        }

        template <typename CB>
        void readLine(boost::asio::streambuf &buff, CB &&ct)
        {
            boost::asio::async_read_until(_input, buff, '\n', std::forward<CB>(ct));
        }

        template <typename Buffer, typename CB>
        void asyncRead(Buffer &buff, CB &&ct)
        {
            _input.async_read_some(boost::asio::buffer(buff.data(), buff.size()), std::forward<CB>(ct));
        }

        template <typename Buffer, typename CB>
        void asyncWrite(Buffer &buff, CB &&ct)
        {
            _input.async_write_some(boost::asio::buffer(buff.data(), buff.size()), std::forward<CB>(ct));
        }

        void close() noexcept
        {
            _input.close();
        }

    private:
        boost::asio::posix::stream_descriptor _input;
    };
}

#endif //SPIDER_SERVER_ASYNCINTERACTIVE_HPP
