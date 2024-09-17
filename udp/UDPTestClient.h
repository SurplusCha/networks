#ifndef IDEA_NETWORKS_UDP_UDPTESTCLIENT_H
#define IDEA_NETWORKS_UDP_UDPTESTCLIENT_H

#include "../Context.h"
#include "UDPClient.h"
#include "IUDPClientListener.h"

namespace idea::networks {
	namespace udp {
		class UDPTestClient : public IUDPClientListener
		{
		public:
			UDPTestClient(Context& ctx);
			UDPTestClient(Context& ctx, const std::string& host, const std::string& port);
			UDPTestClient(Context& ctx, const std::string& host, uint16_t port);
			virtual ~UDPTestClient() = default;
			UDPTestClient(const UDPTestClient& client) = delete;
			UDPTestClient(UDPTestClient&& client) noexcept = delete;
			UDPTestClient& operator=(const UDPTestClient& client) = delete;
			UDPTestClient& operator=(UDPTestClient&& client) noexcept = delete;

		public:
			bool create(const std::string& host, const std::string& port);
			bool create(const std::string& host, uint16_t port);
			bool create();
			bool write();
			bool destroy();

		public:
			bool onReceivedUDPClientData(const std::string& data) override;
			bool onReceivedUDPClientError(const std::string& data) override;

		private:
			std::unique_ptr<idea::networks::udp::UDPClient>					m_udpClient;
		};
	}
}

#endif