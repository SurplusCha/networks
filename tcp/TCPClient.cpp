#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>
#include "TCPClient.h"

namespace idea::networks::tcp {
	TCPClient::TCPClient(boost::asio::io_context& ctx)
		: m_ctx(ctx)
		, m_resolver(ctx)
		, m_socket(ctx)
		, m_session()
		, m_host()
		, m_port()
	{}

	TCPClient::TCPClient(boost::asio::io_context& ctx, const std::string& host, const std::string& port)
		: m_ctx(ctx)
		, m_resolver(ctx)
		, m_socket(ctx)
		, m_session()
		, m_host(host)
		, m_port(boost::lexical_cast<uint16_t>(port))
	{}

	TCPClient::TCPClient(boost::asio::io_context& ctx, const std::string& host, uint16_t port)
		: m_ctx(ctx)
		, m_resolver(ctx)
		, m_socket(ctx)
		, m_session()
		, m_host(host)
		, m_port(port)
	{}

	TCPClient::TCPClient(TCPClient&& client) noexcept
		: m_ctx(client.m_ctx)
		, m_resolver(std::move(client.m_resolver))
		, m_socket(std::move(client.m_socket))
		, m_session(std::move(client.m_session))
		, m_host(std::move(client.m_host))
		, m_port(std::move(client.m_port))
	{}

	bool TCPClient::create(const std::string& host, const std::string& port)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << host << ", " << port;
		m_host = host;
		m_port = boost::lexical_cast<uint16_t>(port);
		return create();
	}

	bool TCPClient::create(const std::string& host, uint16_t port)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << host << ", " << port;
		m_host = host;
		m_port = port;
		return create();
	}

	bool TCPClient::create()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		if (m_host.empty() || m_port == 0) {
			BOOST_LOG_TRIVIAL(error) << "You must fill out right host and port information.";
			return false;
		}

		return true;
	}

	bool TCPClient::connect()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();

		boost::system::error_code ec;
		auto eps = m_resolver.resolve(m_host, std::to_string(m_port), ec);	
		if (!ec) {
			boost::asio::async_connect(m_socket, eps,
				[this](const boost::system::error_code& ec, const boost::asio::ip::tcp::endpoint& ep) {
					if (!ec) {
						BOOST_LOG_TRIVIAL(info) << "Server connect!";
						try {
							auto ep = m_socket.local_endpoint();
							BOOST_LOG_TRIVIAL(info) << "Client local IP Address: " << ep.address().to_string();
							BOOST_LOG_TRIVIAL(info) << "Client local Port: " << ep.port();
						}
						catch (const std::exception& e) {
							BOOST_LOG_TRIVIAL(error) << e.what();
						}

						m_session = std::make_shared<TCPSession>(0, std::move(m_socket));
						m_session->addListener(this);
						m_session->create();
					}
					else {
						BOOST_LOG_TRIVIAL(error) << "Connect error: " << ec.message();
					}
				});
		}
		else {
			boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(m_host), m_port);
			m_socket.async_connect(ep, [this](const boost::system::error_code& ec) {
				if (!ec) {
					BOOST_LOG_TRIVIAL(info) << "Server connect!";
					try {
						auto ep = m_socket.local_endpoint();
						BOOST_LOG_TRIVIAL(info) << "Client local IP Address: " << ep.address().to_string();
						BOOST_LOG_TRIVIAL(info) << "Client local Port: " << ep.port();
					}
					catch (const std::exception& e) {
						BOOST_LOG_TRIVIAL(error) << e.what();
					}

					m_session = std::make_shared<TCPSession>(0, std::move(m_socket));
					m_session->addListener(this);
					m_session->create();
				}
				else {
					BOOST_LOG_TRIVIAL(error) << "Connect error: " << ec.message();
				}
			});
		}

		return true;
	}

	bool TCPClient::write(const std::string& message)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << message;
		if (m_session != nullptr)
			return m_session->write(message);
		else
			return false;
	}

	bool TCPClient::disconnect()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		if (m_session != nullptr) {
			m_session->deleteListener(this);
			m_session->destroy();
		}
		return true;
	}

	bool TCPClient::destroy()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		m_host = "";
		m_port = 0;
		return true;
	}

	bool TCPClient::onReceivedSessionRead(std::size_t channel, const std::string& message)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << channel << " " << message;
		return true;
	}

	bool TCPClient::onReceivedSessionDisconnected(std::size_t channel)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << channel;
		return true;
	}

	bool TCPClient::onReceivedSessionError(std::size_t channel, const std::string& error)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << channel << " " << error;
		return true;
	}
}