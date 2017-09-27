set(SOURCES client-test.cpp)

add_test_core(clientTest client-test ${SOURCES})
target_link_libraries(client-test ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})