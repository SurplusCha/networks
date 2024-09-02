// #define BOOST_USE_WINAPI_VERSION BOOST_WINAPI_VERSION_WIN7

#include <boost/log/trivial.hpp>
#include <boost/asio.hpp>
#include <thread>
#include "TCPServer.h"
#include "UDPServer.h"
#include "TCPClient.h"
#include "UDPClient.h"
#include <chrono>

int main()
{
    BOOST_LOG_TRIVIAL(info) << "Hello Idea";

    try {
        boost::asio::io_context ctx;
        idea::networks::TCPServer tcpServer(ctx);
        tcpServer.create(18500);
        tcpServer.bind();

        idea::networks::UDPServer udpServer(ctx);
        udpServer.create(18510);
        udpServer.bind();

        std::this_thread::sleep_for(std::chrono::seconds(2));

        idea::networks::TCPClient tcpClient(ctx);
        tcpClient.create("127.0.0.1", "18500");
        tcpClient.connect();

        idea::networks::UDPClient udpClient(ctx);
        udpClient.create("127.0.0.1", "18510");

        std::vector<std::thread> threads;
        auto threadCnt = std::thread::hardware_concurrency();
        BOOST_LOG_TRIVIAL(info) << "Threads number: " << threadCnt;
        for (auto i = 0; i < threadCnt; ++i) {
            threads.emplace_back([&ctx]() {
                ctx.run();
            });
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
       
        tcpClient.write("Testest");
        udpClient.write("TESTEST");
        tcpClient.write("1234");
        udpClient.write("9876");

        std::this_thread::sleep_for(std::chrono::seconds(2));

        tcpClient.disconnect();
        tcpClient.destroy();

        udpClient.destroy();

        tcpServer.unbind();
        tcpServer.destroy();

        udpServer.unbind();
        udpServer.destroy();

        std::this_thread::sleep_for(std::chrono::seconds(2));

        for (auto& thread : threads)
            thread.join();
    }
    catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << "Exception: " << e.what();
    }

    return 0;
}
