#include <boost/log/trivial.hpp>
#include <span>
#include "UDPServer.h"
#include "IUDPServerListener.h"

namespace idea::networks::udp {
	UDPServer::UDPServer(boost::asio::io_context& ctx)
		: m_ctx(ctx)
		, m_socket(ctx)
		, m_listeners()
		, m_strand(boost::asio::make_strand(ctx.get_executor()))
		, m_resolver(ctx)
		, m_data()
		, m_writeBuffer()
	{}

	UDPServer::UDPServer(UDPServer&& server) noexcept
		: m_ctx(server.m_ctx)
		, m_socket(std::move(server.m_socket))
		, m_listeners(std::move(server.m_listeners))
		, m_strand(std::move(server.m_strand))
		, m_resolver(std::move(server.m_resolver))
		, m_data(std::move(server.m_data))
		, m_writeBuffer(std::move(server.m_writeBuffer))
	{}

	bool UDPServer::create(int16_t port)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << port;
		try {
			boost::asio::ip::udp::endpoint ep(boost::asio::ip::udp::v4(), port);
			m_socket.open(ep.protocol());
			m_socket.set_option(boost::asio::socket_base::reuse_address(true));
			m_socket.bind(ep);
		}
		catch (const std::exception& e) {
			for (auto listener : m_listeners)
				if (listener != nullptr)
					listener->onReceivedUDPServerError("", 0, e.what());
			return false;
		}

		return true;
	}

	bool UDPServer::bind()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		return onRead();
	}

	bool UDPServer::write(const std::string& address, uint16_t port, const std::string& message)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), address, std::to_string(port));
		auto ep = std::make_shared<boost::asio::ip::udp::endpoint>(*m_resolver.resolve(query));

		boost::asio::post(m_strand, [this, ep, message]() {
			bool writeInProgress = !m_writeBuffer.empty();
			m_writeBuffer.emplace_back(std::make_pair(ep, message));
			if (!writeInProgress)
				this->onWrite();
		});

		return true;
	}

	bool UDPServer::unbind()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		try {
			boost::asio::post(m_strand, [this]() {
				m_socket.cancel();
			});
		}
		catch (const std::exception& e) {
			for (auto listener : m_listeners)
				if (listener != nullptr)
					listener->onReceivedUDPServerError("", 0, e.what());
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
			for (auto listener : m_listeners)
				if (listener != nullptr)
					listener->onReceivedUDPServerError("", 0, e.what());
			return false;
		}

		return true;
	}

	bool UDPServer::onRead()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		try {
			auto ep = std::make_shared<boost::asio::ip::udp::endpoint>();
			m_socket.async_receive_from(boost::asio::buffer(m_data), *ep,
				boost::asio::bind_executor(m_strand, [this, ep](const boost::system::error_code& ec, std::size_t bytesTransferred) {
					if (!ec) {
						auto data = std::span<char>(m_data.data(), bytesTransferred);
						for (auto listener : m_listeners)
							if (listener != nullptr)
								listener->onReceivedUDPServerData(ep->address().to_string(), 
									ep->port(), std::string(std::begin(data), std::end(data)));

						onRead();
					}
					else {
						for (auto listener : m_listeners)
							if (listener != nullptr)
								listener->onReceivedUDPServerError(ep->address().to_string(), ep->port(), ec.message());

						std::this_thread::sleep_for(std::chrono::milliseconds(100));
						if (m_socket.is_open())
							onRead();
					}
					}));
		}
		catch (const std::exception& e) {
			for (auto listener : m_listeners)
				if (listener != nullptr)
					listener->onReceivedUDPServerError("", 0, e.what());
			return false;
		}

		return true;
	}

	bool UDPServer::onWrite()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		m_socket.async_send_to(boost::asio::buffer(m_writeBuffer.front().second), *(m_writeBuffer.front().first),
			boost::asio::bind_executor(m_strand,
				[this](const boost::system::error_code& ec, std::size_t byteTransferred) {
					if (!ec) {
						m_writeBuffer.pop_front();
						if (!m_writeBuffer.empty())
							onWrite();
					}
					else {
						auto& ep = m_writeBuffer.front().first;
						for (auto listener : m_listeners)
							if (listener != nullptr)
								listener->onReceivedUDPServerError(ep->address().to_string(), ep->port(), ec.message());
						m_writeBuffer.pop_front();
					}
		}));
		return true;
	}
}