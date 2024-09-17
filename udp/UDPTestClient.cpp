#include <boost/log/trivial.hpp>
#include "UDPTestClient.h"

namespace idea::networks::udp {
	UDPTestClient::UDPTestClient(Context& ctx)
		: m_udpClient(std::make_unique<UDPClient>(ctx.getContext()))
	{}

	UDPTestClient::UDPTestClient(Context& ctx, const std::string& host, const std::string& port)
		: m_udpClient(std::make_unique<UDPClient>(ctx.getContext(), host, port))
	{}

	UDPTestClient::UDPTestClient(Context& ctx, const std::string& host, uint16_t port)
		: m_udpClient(std::make_unique<UDPClient>(ctx.getContext(), host, port))
	{}

	bool UDPTestClient::create(const std::string& host, const std::string& port)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		m_udpClient->addListener(this);
		return m_udpClient->create(host, port);
	}

	bool UDPTestClient::create(const std::string& host, uint16_t port)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		m_udpClient->addListener(this);
		return m_udpClient->create(host, port);
	}

	bool UDPTestClient::create()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		m_udpClient->addListener(this);
		return m_udpClient->create();
	}

	bool UDPTestClient::write()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		m_udpClient->write("98765");
		m_udpClient->write("");
		m_udpClient->write("TSETSETSET");

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		m_udpClient->write("5678");
		m_udpClient->write("TESTESTESTTEST");
		return true;
	}

	bool UDPTestClient::destroy()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		m_udpClient->deleteListener(this);
		return m_udpClient->destroy();
	}

	bool UDPTestClient::onReceivedUDPClientData(const std::string& data)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		BOOST_LOG_TRIVIAL(trace) << "UDPTestClient Results: " << data;
		return true;
	}

	bool UDPTestClient::onReceivedUDPClientError(const std::string& data)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		BOOST_LOG_TRIVIAL(error) << data;
		return true;
	}
}