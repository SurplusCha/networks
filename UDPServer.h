#ifndef IDEA_NETWORKS_UDPSERVER_H
#define IDEA_NETWORKS_UDPSERVER_H

#include <boost/asio.hpp>
#include <array>

namespace idea::networks {
	class UDPServer
	{
	public:
		UDPServer(boost::asio::io_context& ctx);
		virtual ~UDPServer() = default;
		UDPServer(const UDPServer& server) = delete;
		UDPServer(UDPServer&& server) noexcept;
		UDPServer& operator=(const UDPServer& server) = delete;
		UDPServer& operator=(UDPServer&& server) noexcept = delete;

	public:
		bool create(int16_t port);
		bool bind();
		bool write(const std::string& address, uint16_t port, const std::string& message);
		bool unbind();
		bool destroy();

	private:
		bool onRead(const boost::asio::ip::udp::endpoint& ep, const boost::system::error_code& ec, std::size_t byteTransferred);
		bool onWrite(const boost::asio::ip::udp::endpoint& ep, const boost::system::error_code& ec, std::size_t byteTransferred);

	private:
		boost::asio::io_context&							m_ctx;
		boost::asio::ip::udp::socket						m_socket;
		boost::asio::strand<boost::asio::any_io_executor>	m_strand;
		boost::asio::ip::udp::resolver						m_resolver;
		std::array<char, 1024>								m_data;
	};
}

#endif
