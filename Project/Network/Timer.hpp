//
// Created by roman sztergbaum on 30/10/2017.
//

#ifndef SPIDER_CLIENT_TIMER_HPP
#define SPIDER_CLIENT_TIMER_HPP

#include <boost/asio/deadline_timer.hpp>
#include <Network/IOManager.hpp>

namespace spi::net
{
    class Timer
    {
    public:
        using internalT = boost::asio::deadline_timer;

        explicit Timer(net::IOManager &mgr, long seconds) noexcept :
            _timer{mgr.get(), boost::posix_time::seconds(seconds)}
        {
        }

        ~Timer() noexcept = default;

        void setExpiry(long seconds) noexcept
        {
            _timer.expires_from_now(boost::posix_time::seconds(seconds));
        }

        template <typename CallbackT>
        void asyncWait(CallbackT &&cb) noexcept
        {
            _timer.async_wait(std::forward<CallbackT>(cb));
        }

    private:
        internalT _timer;
    };
}

#endif //SPIDER_CLIENT_TIMER_HPP
