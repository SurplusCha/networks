#include "EchoServer.h"
#include "TCPServer.h"

namespace idea::networks::tcp {
	EchoServer::EchoServer(Context& ctx)
		: m_tcpServer(std::make_unique<TCPServer>(ctx.getContext()))
	{}

	bool EchoServer::create(uint16_t port)
	{
		m_tcpServer->addListener(this);
		return m_tcpServer->create(port);
	}

	bool EchoServer::bind()
	{
		return m_tcpServer->bind();
	}

	bool EchoServer::unbind()
	{
		return m_tcpServer->unbind();
	}

	bool EchoServer::destroy()
	{
		m_tcpServer->deleteListsener(this);
		return m_tcpServer->destroy();
	}

	bool EchoServer::onTCPServerBound(std::size_t channel)
	{
		return true;
	}

	bool EchoServer::onReceivedTCPServerData(std::size_t channel, const std::string& data)
	{
		m_tcpServer->write(channel, data);
		return true;
	}

	bool EchoServer::onReceivedTCPServerError(std::size_t channel, const std::string& data)
	{
		return true;
	}

	bool EchoServer::onTCPServerUnbound(std::size_t channel)
	{
		return true;
	}
}