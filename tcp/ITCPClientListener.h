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
		virtual bool onTCPClientConnected() = 0;
		virtual bool onReceivedTCPClientData(const std::string& data) = 0;
		virtual bool onReceivedTCPClientError(const std::string& data) = 0;
		virtual bool onTCPClientDisconnected() = 0;
	};
}

#endif
