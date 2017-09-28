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
        ErrorCode(const boost::system::error_code &ec) noexcept : _ec(ec)
        {
        }

        bool operator!() const noexcept
        {
            return !_ec;
        }

    private:
        const boost::system::error_code _ec;
    };

    namespace net
    {
        static constexpr const auto ErrorPlaceholder = boost::asio::placeholders::error;
        static constexpr const auto BytesTransferredPlaceholder = boost::asio::placeholders::bytes_transferred;
    }
}

#endif //SPIDER_SERVER_ERRORCODE_HPP
