#ifndef IDEA_NETWORKS_UDP_UDPCLIENT_H
#define IDEA_NETWORKS_UDP_UDPCLIENT_H

#include <boost/asio.hpp>
#include <deque>

namespace idea::networks::udp {
	class UDPClient
	{
	public:
		UDPClient(boost::asio::io_context& ctx);
		UDPClient(boost::asio::io_context& ctx, const std::string& host, const std::string& port);
		UDPClient(boost::asio::io_context& ctx, const std::string& host, uint16_t port);
		~UDPClient() = default;
		UDPClient(const UDPClient& client) = delete;
		UDPClient(UDPClient&& client) noexcept;
		UDPClient& operator=(const UDPClient& client) = delete;
		UDPClient& operator=(UDPClient&& client) noexcept = delete;

	public:
		bool create(const std::string& host, const std::string& port);
		bool create(const std::string& host, uint16_t port);
		bool create();
		bool write(const std::string& message);
		bool destroy();

	private:
		bool onWrite();
		bool onRead();

	private:
		boost::asio::io_context&							m_ctx;
		boost::asio::strand<boost::asio::any_io_executor>	m_strand;
		boost::asio::ip::udp::resolver						m_resolver;
		boost::asio::ip::udp::socket						m_socket;
		std::string											m_host;
		uint16_t											m_port;
		std::array<char, 1024>								m_data;
		std::deque<std::string>								m_writeBuffer;
	};
}

#endif

