#ifndef IDEA_NETWORKS_TCP_ECHOSERVER_H
#define IDEA_NETWORKS_TCP_ECHOSERVER_H

#include "../Context.h"
#include "ITCPServerListener.h"

namespace idea::networks {
	class TCPServer;
	namespace tcp {
		class EchoServer : public ITCPServerListener
		{
		public:
			EchoServer(Context& ctx);
			virtual ~EchoServer() = default;
			EchoServer(const EchoServer& server) = delete;
			EchoServer(EchoServer&& server) noexcept = delete;
			EchoServer& operator=(const EchoServer& server) = delete;
			EchoServer& operator=(EchoServer&& server) noexcept = delete;

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
			std::unique_ptr<idea::networks::TCPServer>					m_tcpServer;
		};
	}
}

#endif
