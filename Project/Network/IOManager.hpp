//
// Created by doom on 28/09/17.
//

#ifndef SPIDER_SERVER_IOSERVICE_HPP
#define SPIDER_SERVER_IOSERVICE_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <Network/ErrorCode.hpp>

namespace asio = boost::asio;

namespace spi::net
{
    class IOManager
    {
    public:
        using InternalT = asio::io_service;

        InternalT &get() noexcept
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

        void onTerminationSignals(const std::function<void()> &func)
        {
            _signals.add(SIGINT);
            _signals.add(SIGTERM);

            _signals.async_wait([func]([[maybe_unused]] const ErrorCode &ec, [[maybe_unused]] int sig) {
                func();
            });
        }

    private:
        InternalT _service;
        asio::signal_set _signals{_service};
    };
}

#endif //SPIDER_SERVER_IOSERVICE_HPP
