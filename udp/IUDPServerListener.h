#ifndef IDEA_NETWORKS_UDP_IUDPSERVERLISTENER_H
#define IDEA_NETWORKS_UDP_IUDPSERVERLISTENER_H

#include <string>

namespace idea::networks::udp
{
	class IUDPServerListener {
	public:
		IUDPServerListener() = default;
		virtual ~IUDPServerListener() = default;

	public:
		virtual bool onReceivedUDPServerData(const std::string& ip, uint16_t port, const std::string& data) = 0;
		virtual bool onReceivedUDPServerError(const std::string& ip, uint16_t port, const std::string& data) = 0;
	};
}

#endif