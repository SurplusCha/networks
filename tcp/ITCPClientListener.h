#ifndef IDEA_NETWORKS_TCP_ITCPCLIENTLISTENER_H
#define IDEA_NETWORKS_TCP_ITCPCLIENTLISTENER_H

#include <string>

namespace idea::networks::tcp
{
	class ITCPClientListener {
	public:
		ITCPClientListener() = default;
		virtual ~ITCPClientListener() = default;

	public:
		virtual bool onTCPClientConnected(std::size_t channel) = 0;
		virtual bool onReceivedTCPClientData(std::size_t channel, const std::string& data) = 0;
		virtual bool onReceivedTCPClientError(std::size_t channel, const std::string& data) = 0;
		virtual bool onTCPClientDisconnected(std::size_t channel) = 0;
	};
}

#endif
