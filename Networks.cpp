// #define BOOST_USE_WINAPI_VERSION BOOST_WINAPI_VERSION_WIN7

#include <boost/log/trivial.hpp>
#include <boost/asio.hpp>
#include <thread>
#include "tcp/TCPEchoServer.h"
#include "udp/UDPEchoServer.h"
#include "tcp/TCPTestClient.h"
#include "udp/UDPTestClient.h"
#include "Context.h"
#include <chrono>

int main()
{
    BOOST_LOG_TRIVIAL(info) << "Hello Idea";

    try {
        idea::networks::Context tcpServerContext;
        idea::networks::tcp::TCPEchoServer tcpServer(tcpServerContext);
        tcpServer.create(18500);
        tcpServer.bind();

        idea::networks::Context udpServerContext;
        idea::networks::udp::UDPEchoServer udpServer(udpServerContext);
        udpServer.create(18510);
        udpServer.bind();

        std::this_thread::sleep_for(std::chrono::seconds(2));

        idea::networks::Context tcpClientContext;
        idea::networks::tcp::TCPTestClient tcpClient(tcpClientContext);
        tcpClient.create("127.0.0.1", "18500");
        tcpClient.connect();

        idea::networks::Context udpClientContext;
        idea::networks::udp::UDPTestClient udpClient(udpClientContext);
        udpClient.create("127.0.0.1", "18510");

        tcpServerContext.run();
        udpServerContext.run();
        tcpClientContext.run();
        udpClientContext.run();
        std::this_thread::sleep_for(std::chrono::seconds(2));
       
        tcpClient.write();
        udpClient.write();

        std::this_thread::sleep_for(std::chrono::seconds(2));

        tcpClient.disconnect();
        tcpClient.destroy();

        udpClient.destroy();

        tcpServer.unbind();
        tcpServer.destroy();

        udpServer.unbind();
        udpServer.destroy();

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << "Exception: " << e.what();
    }

    return 0;
}
