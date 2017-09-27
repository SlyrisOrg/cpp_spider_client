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

set(SOURCES_SPIDER Project/main.cpp)

add_executable(${PROJECT_NAME} ${SOURCES_SPIDER})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_subdirectory(test)
