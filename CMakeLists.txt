cmake_minimum_required(VERSION 3.20)
project(networks)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set Boost
set(BOOST_ROOT /home/systembug/workspace/boost/boost_1_86_0)
set(BOOST_INCLUDEDIR ${BOOST_ROOT})
set(BOOST_LIBRARYDIR ${BOOST_ROOT}/stage/lib)

find_package(Boost COMPONENTS system filesystem log log_setup thread REQUIRED)

if (Boost_FOUND)
    message(STATUS "Boost found successfully!")
    message(STATUS "Boost's include directory: " ${Boost_INCLUDE_DIRS})
    include_directories(${Boost_INCLUDE_DIRS})
    message(STATUS "Boost's library directory: " ${Boost_LIBRARY_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

set(CMAKE_CXX_FLAGS "-DBOOST_LOG_DYN_LINK")
set(TCP
        ${CMAKE_SOURCE_DIR}/tcp/TCPServer.cpp
        ${CMAKE_SOURCE_DIR}/tcp/TCPClient.cpp
        ${CMAKE_SOURCE_DIR}/tcp/TCPSession.cpp
        ${CMAKE_SOURCE_DIR}/tcp/TCPTestClient.cpp
        ${CMAKE_SOURCE_DIR}/tcp/TCPEchoServer.cpp
)
set(UDP
        ${CMAKE_SOURCE_DIR}/udp/UDPServer.cpp
        ${CMAKE_SOURCE_DIR}/udp/UDPClient.cpp
        ${CMAKE_SOURCE_DIR}/udp/UDPTestClient.cpp
        ${CMAKE_SOURCE_DIR}/udp/UDPEchoServer.cpp
)

add_executable(networks Networks.cpp Utility.cpp Context.cpp ${TCP} ${UDP})
target_link_libraries(networks ${Boost_LIBRARIES} Boost::log Boost::system Boost::filesystem Boost::thread)

set(BOOST_LIBRARY_FILES
        ${BOOST_LIBRARYDIR}/libboost_filesystem.so.1.86.0
        ${BOOST_LIBRARYDIR}/libboost_thread.so.1.86.0
)

foreach(file ${BOOST_LIBRARY_FILES})
    add_custom_command(
            TARGET networks
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${file}
            ${CMAKE_BINARY_DIR}
            COMMENT "Copying ${file} to run binary after build"
    )
endforeach()