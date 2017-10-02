//
// Created by doom on 28/09/17.
//

#ifndef SPIDER_SERVER_ERRORCODE_HPP
#define SPIDER_SERVER_ERRORCODE_HPP

#include <boost/system/error_code.hpp>
#include <boost/asio/placeholders.hpp>

namespace spi
{
    class ErrorCode
    {
    public:
        using InternalT = boost::system::error_code;

        ErrorCode(const InternalT &ec) noexcept : _ec(ec)
        {
        }

        bool operator!() const noexcept
        {
            return !_ec;
        }

        operator bool() const noexcept
        {
            return (bool)_ec;
        }

        const InternalT &get() const noexcept
        {
            return _ec;
        }

        std::string message() const noexcept
        {
            return _ec.message();
        }

    private:
        const InternalT _ec;
    };

    namespace net
    {
        static constexpr const auto ErrorPlaceholder = boost::asio::placeholders::error;
        static constexpr const auto BytesTransferredPlaceholder = boost::asio::placeholders::bytes_transferred;
    }
}

#endif //SPIDER_SERVER_ERRORCODE_HPP
