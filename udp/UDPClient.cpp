#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>
#include <span>
#include "UDPClient.h"

namespace idea::networks::udp {
	UDPClient::UDPClient(boost::asio::io_context& ctx)
		: m_ctx(ctx)
		, m_strand(boost::asio::make_strand(ctx.get_executor()))
		, m_resolver(ctx)
		, m_socket(ctx)
		, m_host()
		, m_port()
		, m_data()
		, m_writeBuffer()
	{}

	UDPClient::UDPClient(boost::asio::io_context& ctx, const std::string& host, const std::string& port)
		: m_ctx(ctx)
		, m_strand(boost::asio::make_strand(ctx.get_executor()))
		, m_resolver(ctx)
		, m_socket(ctx)
		, m_host(host)
		, m_port(boost::lexical_cast<uint16_t>(port))
		, m_data()
		, m_writeBuffer()
	{}

	UDPClient::UDPClient(boost::asio::io_context& ctx, const std::string& host, uint16_t port)
		: m_ctx(ctx)
		, m_strand(boost::asio::make_strand(ctx.get_executor()))
		, m_resolver(ctx)
		, m_socket(ctx)
		, m_host(host)
		, m_port(port)
		, m_data()
		, m_writeBuffer()
	{}

	UDPClient::UDPClient(UDPClient&& client) noexcept
		: m_ctx(client.m_ctx)
		, m_strand(std::move(client.m_strand))
		, m_resolver(std::move(client.m_resolver))
		, m_socket(std::move(client.m_socket))
		, m_host(std::move(client.m_host))
		, m_port(std::move(client.m_port))
		, m_data(std::move(client.m_data))
		, m_writeBuffer(std::move(client.m_writeBuffer))
	{}

	bool UDPClient::create(const std::string& host, const std::string& port)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << host << ", " << port;
		m_host = host;
		m_port = boost::lexical_cast<uint16_t>(port);
		return create();
	}

	bool UDPClient::create(const std::string& host, uint16_t port)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << host << ", " << port;
		m_host = host;
		m_port = port;
		return create();
	}

	bool UDPClient::create()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		if (m_host.empty() || m_port == 0) {
			BOOST_LOG_TRIVIAL(error) << "You must fill out right host and port information.";
			return false;
		}

		try {
			m_socket.open(boost::asio::ip::udp::v4());
			m_socket.set_option(boost::asio::socket_base::reuse_address(true));
		}
		catch (const std::exception& e) {
			BOOST_LOG_TRIVIAL(error) << e.what();
			return false;
		}

		return onRead();
	}

	bool UDPClient::write(const std::string& message)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();

		boost::asio::post(m_strand, [this, message]() {
			bool writeInProgress = !m_writeBuffer.empty();
			m_writeBuffer.emplace_back(message);
			if (!writeInProgress)
				onWrite();
			});
		return true;
	}

	bool UDPClient::destroy()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		try {
			m_socket.cancel();
			m_socket.close();
		}
		catch (const std::exception& e) {
			BOOST_LOG_TRIVIAL(error) << e.what();
			return false;
		}

		return true;
	}

	bool UDPClient::onWrite()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		boost::asio::ip::udp::endpoint ep(boost::asio::ip::make_address(m_host), m_port);

		m_socket.async_send_to(boost::asio::buffer(m_writeBuffer.front()), ep,
			boost::asio::bind_executor(m_strand,
				[this, ep](const boost::system::error_code& ec, std::size_t byteTransferred) {
					if (!ec) {
						BOOST_LOG_TRIVIAL(trace) << "Send message..";
						m_writeBuffer.pop_front();
						if (!m_writeBuffer.empty())
							onWrite();
					}
					else {
						BOOST_LOG_TRIVIAL(error) << ec.message();
					}
				}));

		return true;
	}

	bool UDPClient::onRead()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		try {
			auto ep = std::make_shared<boost::asio::ip::udp::endpoint>();
			m_socket.async_receive_from(boost::asio::buffer(m_data), *ep,
				boost::asio::bind_executor(m_strand, [this, ep](const boost::system::error_code& ec, std::size_t bytesTransferred) {
					if (!ec) {
						auto data = std::span<char>(m_data.data(), bytesTransferred);
						BOOST_LOG_TRIVIAL(trace) << std::string(std::begin(data), std::end(data));
					}
					else {
						BOOST_LOG_TRIVIAL(error) << ec.message();
						std::this_thread::sleep_for(std::chrono::milliseconds(100));
					}
					onRead();
				}));
		}
		catch (const std::exception& e) {
			BOOST_LOG_TRIVIAL(error) << e.what();
			return false;
		}

		return true;
	}
}