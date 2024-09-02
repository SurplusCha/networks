#include <boost/log/trivial.hpp>
#include "UDPServer.h"

namespace idea::networks {
	UDPServer::UDPServer(boost::asio::io_context& ctx)
		: m_ctx(ctx)
		, m_socket(ctx)
		, m_strand(boost::asio::make_strand(ctx.get_executor()))
		, m_resolver(ctx)
		, m_data()
	{}

	UDPServer::UDPServer(UDPServer&& server) noexcept
		: m_ctx(server.m_ctx)
		, m_socket(std::move(server.m_socket))
		, m_strand(std::move(server.m_strand))
		, m_resolver(std::move(server.m_resolver))
		, m_data(std::move(server.m_data))
	{}

	bool UDPServer::create(int16_t port)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << port;
		try {
			boost::asio::ip::udp::endpoint ep(boost::asio::ip::udp::v4(), port);
			m_socket.open(ep.protocol());
			m_socket.bind(ep);
		}
		catch (const std::exception& e) {
			BOOST_LOG_TRIVIAL(error) << e.what();
			return false;
		}

		return true;
	}

	bool UDPServer::bind()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		boost::asio::post(m_strand, [this]() {
			boost::asio::ip::udp::endpoint ep;
			m_socket.async_receive_from(boost::asio::buffer(m_data), ep,
				boost::asio::bind_executor(m_strand,
					[this, ep](const boost::system::error_code& ec, std::size_t byteTransferred) {
						this->onRead(ep, ec, byteTransferred);
			}));
		});
			
		return true;
	}

	bool UDPServer::write(const std::string& address, uint16_t port, const std::string& message)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), address, std::to_string(port));
		auto& ep = *m_resolver.resolve(query);
		m_socket.async_send_to(boost::asio::buffer(m_data), ep,
			boost::asio::bind_executor(m_strand,
				[this, ep](const boost::system::error_code& error, std::size_t byteTransferred) {
					this->onWrite(ep, error, byteTransferred);
				}));
		return true;
	}

	bool UDPServer::unbind()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		try {
			boost::asio::post(m_ctx, [this]() {
				m_socket.cancel();
			});
		}
		catch (const std::exception& e) {
			BOOST_LOG_TRIVIAL(error) << e.what();
			return false;
		}
		return true;
	}

	bool UDPServer::destroy()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		try {
			boost::asio::post(m_strand, [this]() {
				m_socket.close();
			});
		}
		catch (const std::exception& e) {
			BOOST_LOG_TRIVIAL(error) << e.what();
			return false;
		}

		return true;
	}

	bool UDPServer::onRead(const boost::asio::ip::udp::endpoint& ep, const boost::system::error_code& ec, std::size_t byteTransferred)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		if (!ec || ec == boost::asio::error::message_size) {
			BOOST_LOG_TRIVIAL(trace) << m_data.data();
			bind();
		}
		else {
			BOOST_LOG_TRIVIAL(error) << ec.message();
			return false;
		}

		return true;
	}

	bool UDPServer::onWrite(const boost::asio::ip::udp::endpoint& ep, const boost::system::error_code& ec, std::size_t byteTransferred)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		return true;
	}
}