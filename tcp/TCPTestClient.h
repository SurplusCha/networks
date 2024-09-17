#ifndef IDEA_NETWORKS_TCP_TCPTESTCLIENT_H
#define IDEA_NETWORKS_TCP_TCPTESTCLIENT_H

#include "../Context.h"
#include "TCPClient.h"
#include "ITCPClientListener.h"

namespace idea::networks {
	namespace tcp {
		class TCPTestClient : public ITCPClientListener
		{
		public:
			TCPTestClient(Context& ctx);
			TCPTestClient(Context& ctx, const std::string& host, const std::string& port);
			TCPTestClient(Context& ctx, const std::string& host, uint16_t port);
			virtual ~TCPTestClient() = default;
			TCPTestClient(const TCPTestClient& client) = delete;
			TCPTestClient(TCPTestClient&& client) noexcept = delete;
			TCPTestClient& operator=(const TCPTestClient& client) = delete;
			TCPTestClient& operator=(TCPTestClient&& client) noexcept = delete;

		public:
			bool create(const std::string& host, const std::string& port);
			bool create(const std::string& host, uint16_t port);
			bool create();
			bool connect();
			bool write();
			bool disconnect();
			bool destroy();

		public:
			bool onTCPClientConnected() override;
			bool onReceivedTCPClientData(const std::string& data) override;
			bool onReceivedTCPClientError(const std::string& data) override;
			bool onTCPClientDisconnected() override;

		private:
			std::unique_ptr<idea::networks::tcp::TCPClient>					m_tcpClient;
		};
	}
}

#endif
