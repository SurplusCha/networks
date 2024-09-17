#ifndef IDEA_NETWORKS_TCP_TCPECHOSERVER_H
#define IDEA_NETWORKS_TCP_TCPECHOSERVER_H

#include "../Context.h"
#include "TCPServer.h"
#include "ITCPServerListener.h"

namespace idea::networks {
	namespace tcp {
		class TCPEchoServer : public ITCPServerListener
		{
		public:
			TCPEchoServer(Context& ctx);
			virtual ~TCPEchoServer() = default;
			TCPEchoServer(const TCPEchoServer& server) = delete;
			TCPEchoServer(TCPEchoServer&& server) noexcept = delete;
			TCPEchoServer& operator=(const TCPEchoServer& server) = delete;
			TCPEchoServer& operator=(TCPEchoServer&& server) noexcept = delete;

		public:
			bool create(uint16_t port);
			bool bind();
			bool unbind();
			bool destroy();

		public:
			bool onTCPServerBound(std::size_t channel) override;
			bool onReceivedTCPServerData(std::size_t channel, const std::string& data) override;
			bool onReceivedTCPServerError(std::size_t channel, const std::string& data) override;
			bool onTCPServerUnbound(std::size_t channel) override;

		private:
			std::unique_ptr<idea::networks::tcp::TCPServer>					m_tcpServer;
		};
	}
}

#endif
