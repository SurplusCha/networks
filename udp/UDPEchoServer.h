#ifndef IDEA_NETWORKS_UDP_UDPECHOSERVER_H
#define IDEA_NETWORKS_UDP_UDPECHOSERVER_H

#include "../Context.h"
#include "UDPServer.h"
#include "IUDPServerListener.h"

namespace idea::networks {
	namespace udp {
		class UDPEchoServer : public IUDPServerListener
		{
		public:
			UDPEchoServer(Context& ctx);
			virtual ~UDPEchoServer() = default;
			UDPEchoServer(const UDPEchoServer& server) = delete;
			UDPEchoServer(UDPEchoServer&& server) noexcept = delete;
			UDPEchoServer& operator=(const UDPEchoServer& server) = delete;
			UDPEchoServer& operator=(UDPEchoServer&& server) noexcept = delete;

		public:
			bool create(uint16_t port);
			bool bind();
			bool unbind();
			bool destroy();

		public:
			bool onReceivedUDPServerData(const std::string& ip, uint16_t port, const std::string& data) override;
			bool onReceivedUDPServerError(const std::string& ip, uint16_t port, const std::string& data) override;

		private:
			std::unique_ptr<idea::networks::udp::UDPServer>					m_udpServer;
		};
	}
}

#endif