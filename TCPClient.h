#ifndef IDEA_NETWORKS_TCPCLIENT_H
#define IDEA_NETWORKS_TCPCLIENT_H

#include <memory>
#include <boost/asio.hpp>
#include "TCPSession.h"
#include "ISessionListener.h"

namespace idea::networks {
	class TCPClient : public std::enable_shared_from_this<TCPClient>, ISessionListener
	{
	public:
		TCPClient(boost::asio::io_context& ctx);
		TCPClient(boost::asio::io_context& ctx, const std::string& host, const std::string& port);
		TCPClient(boost::asio::io_context& ctx, const std::string& host, uint16_t port);
		~TCPClient() = default;
		TCPClient(const TCPClient& client) = delete;
		TCPClient(TCPClient&& client) noexcept;
		TCPClient& operator=(const TCPClient& client) = delete;
		TCPClient& operator=(TCPClient&& client) noexcept = delete;

	public:
		bool create(const std::string& host, const std::string& port);
		bool create(const std::string& host, uint16_t port);
		bool create();
		bool connect();
		bool write(const std::string& message);
		bool disconnect();
		bool destroy();

	public:
		bool onReceivedSessionRead(std::size_t channel, const std::string& message) override;
		bool onReceivedSessionDisconnected(std::size_t channel) override;
		bool onReceivedSessionError(std::size_t channel, const std::string& error) override;

	private:
		boost::asio::io_context&					m_ctx;
		boost::asio::ip::tcp::resolver				m_resolver;
		boost::asio::ip::tcp::socket				m_socket;
		std::shared_ptr<TCPSession>					m_session;
		std::string									m_host;
		uint16_t									m_port;
	};
}

#endif