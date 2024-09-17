#include <boost/log/trivial.hpp>
#include "TCPEchoServer.h"

namespace idea::networks::tcp {
	TCPEchoServer::TCPEchoServer(Context& ctx)
		: m_tcpServer(std::make_unique<TCPServer>(ctx.getContext()))
	{}

	bool TCPEchoServer::create(uint16_t port)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << port;
		m_tcpServer->addListener(this);
		return m_tcpServer->create(port);
	}

	bool TCPEchoServer::bind()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		return m_tcpServer->bind();
	}

	bool TCPEchoServer::unbind()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		return m_tcpServer->unbind();
	}

	bool TCPEchoServer::destroy()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		m_tcpServer->deleteListsener(this);
		return m_tcpServer->destroy();
	}

	bool TCPEchoServer::onTCPServerBound(std::size_t channel)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << channel;
		return true;
	}

	bool TCPEchoServer::onReceivedTCPServerData(std::size_t channel, const std::string& data)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << channel << " " << data;
		m_tcpServer->write(channel, data);
		return true;
	}

	bool TCPEchoServer::onReceivedTCPServerError(std::size_t channel, const std::string& data)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << channel << " " << data;
		return true;
	}

	bool TCPEchoServer::onTCPServerUnbound(std::size_t channel)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << channel;
		return true;
	}
}