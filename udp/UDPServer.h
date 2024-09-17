#ifndef IDEA_NETWORKS_UDP_UDPSERVER_H
#define IDEA_NETWORKS_UDP_UDPSERVER_H

#include <boost/asio.hpp>
#include <array>
#include <deque>

namespace idea::networks::udp {
	class IUDPServerListener;
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
		inline bool addListener(IUDPServerListener* listener) {
			if (std::find(std::begin(m_listeners), std::end(m_listeners), listener) == std::end(m_listeners)) {
				m_listeners.emplace_back(listener);
				return true;
			}
			return false;
		}
		inline bool deleteListsener(IUDPServerListener* listener) {
			if (std::find(std::begin(m_listeners), std::end(m_listeners), listener) != std::end(m_listeners))
				m_listeners.erase(std::remove(std::begin(m_listeners), std::end(m_listeners), listener), std::end(m_listeners));
			return true;
		}

	public:
		bool create(int16_t port);
		bool bind();
		bool write(const std::string& address, uint16_t port, const std::string& message);
		bool unbind();
		bool destroy();

	private:
		bool onRead(const boost::asio::ip::udp::endpoint& ep, const boost::system::error_code& ec, std::size_t byteTransferred);
		bool onWrite();

	private:
		boost::asio::io_context&																				m_ctx;
		boost::asio::ip::udp::socket																			m_socket;
		std::vector<IUDPServerListener*>																		m_listeners;
		boost::asio::strand<boost::asio::any_io_executor>														m_strand;
		boost::asio::ip::udp::resolver																			m_resolver;
		std::array<char, 1024>																					m_data;
		std::deque<std::pair<std::shared_ptr<boost::asio::ip::udp::endpoint>, std::string>>						m_writeBuffer;
	};
}

#endif
