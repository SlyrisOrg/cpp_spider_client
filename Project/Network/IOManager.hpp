//
// Created by doom on 28/09/17.
//

#ifndef SPIDER_SERVER_IOSERVICE_HPP
#define SPIDER_SERVER_IOSERVICE_HPP

#include <boost/asio/io_service.hpp>

namespace asio = boost::asio;

namespace spi::net
{
    class IOService
    {
    public:
        auto &get()
        {
            return _service;
        }

        void run()
        {
            _service.run();
        }

        void stop()
        {
            _service.stop();
        }

    private:
        asio::io_service _service;
    };
}

#endif //SPIDER_SERVER_IOSERVICE_HPP
