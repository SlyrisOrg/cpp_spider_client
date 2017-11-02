//
// Created by doom on 02/10/17.
//

#ifndef SPIDER_SERVER_ABSTRACTLOGHANDLER_HPP
#define SPIDER_SERVER_ABSTRACTLOGHANDLER_HPP

#include <string>
#include <utils/NonCopyable.hpp>
#include <Utils/ILoggable.hpp>
#include <Network/IOManager.hpp>

namespace spi
{
    class AbstractLogHandle : public utils::NonCopyable
    {
    public:
        virtual ~AbstractLogHandle() noexcept = default;

        /** Add an entry to the log */
        virtual void appendEntry(const ILoggable &l) = 0;

        /** Flush potentially buffered data */
        virtual void flush() = 0;

        virtual void setRoot(const std::string &root) noexcept = 0;

        virtual void setID(const std::string &id) noexcept = 0;

        virtual void setIOManager(net::IOManager &mgr) noexcept = 0;

        /** Setup the handle. After this call, it must be ready to receive entries */
        virtual bool setup() noexcept = 0;
    };
}

#endif //SPIDER_SERVER_ABSTRACTLOGHANDLER_HPP
