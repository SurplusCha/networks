#ifndef IDEA_NETWORKS_UDP_IUDPCLIENTLISTENER_H
#define IDEA_NETWORKS_UDP_IUDPCLIENTLISTENER_H

#include <string>

namespace idea::networks::udp
{
	class IUDPClientListener {
	public:
		IUDPClientListener() = default;
		virtual ~IUDPClientListener() = default;

	public:
		virtual bool onReceivedUDPClientData(const std::string& data) = 0;
		virtual bool onReceivedUDPClientError(const std::string& data) = 0;
	};
}

#endif
