#include <boost/log/trivial.hpp>
#include "UDPEchoServer.h"

namespace idea::networks::udp {
	UDPEchoServer::UDPEchoServer(Context& ctx)
		: m_udpServer(std::make_unique<UDPServer>(ctx.getContext()))
	{}

	bool UDPEchoServer::create(uint16_t port)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << port;
		m_udpServer->addListener(this);
		return m_udpServer->create(port);
		return true;
	}

	bool UDPEchoServer::bind()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		return m_udpServer->bind();
	}

	bool UDPEchoServer::unbind()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		return m_udpServer->unbind();
	}

	bool UDPEchoServer::destroy()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		m_udpServer->deleteListsener(this);
		return m_udpServer->destroy();
	}

	bool UDPEchoServer::onReceivedUDPServerData(const std::string& ip, uint16_t port, const std::string& data)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << ip << " " << port << " " << data;
		m_udpServer->write(ip, port, data);
		return true;
	}

	bool UDPEchoServer::onReceivedUDPServerError(const std::string& ip, uint16_t port, const std::string& data)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << ip << " " << port << " " << data;
		return true;
	}
}