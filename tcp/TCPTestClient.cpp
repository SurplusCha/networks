#include <boost/log/trivial.hpp>
#include "TCPTestClient.h"

namespace idea::networks::tcp {
	TCPTestClient::TCPTestClient(Context& ctx)
		: m_tcpClient(std::make_unique<TCPClient>(ctx.getContext()))
	{}

	TCPTestClient::TCPTestClient(Context& ctx, const std::string& host, const std::string& port)
		: m_tcpClient(std::make_unique<TCPClient>(ctx.getContext(), host, port))
	{}

	TCPTestClient::TCPTestClient(Context& ctx, const std::string& host, uint16_t port)
		: m_tcpClient(std::make_unique<TCPClient>(ctx.getContext(), host, port))
	{}

	bool TCPTestClient::create(const std::string& host, const std::string& port)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		m_tcpClient->addListener(this);
		return m_tcpClient->create(host, port);
	}

	bool TCPTestClient::create(const std::string& host, uint16_t port)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		m_tcpClient->addListener(this);
		return m_tcpClient->create(host, port);
	}

	bool TCPTestClient::create()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		m_tcpClient->addListener(this);
		return m_tcpClient->create();
	}

	bool TCPTestClient::connect()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		return m_tcpClient->connect();
	}

	bool TCPTestClient::write()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		m_tcpClient->write("test");
		m_tcpClient->write("1234");

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		m_tcpClient->write("43210");
		m_tcpClient->write("tset");
		return true;
	}

	bool TCPTestClient::disconnect()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		return m_tcpClient->disconnect();
	}

	bool TCPTestClient::destroy()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		m_tcpClient->deleteListener(this);
		return m_tcpClient->destroy();
	}

	bool TCPTestClient::onTCPClientConnected()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		return true;
	}

	bool TCPTestClient::onReceivedTCPClientData(const std::string& data)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		BOOST_LOG_TRIVIAL(trace) << "TCPTestClient Results: " << data;
		return true;
	}

	bool TCPTestClient::onReceivedTCPClientError(const std::string& data)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		BOOST_LOG_TRIVIAL(error) << data;
		return true;
	}

	bool TCPTestClient::onTCPClientDisconnected()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		return true;
	}
}