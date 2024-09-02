#ifndef IDEA_NETWORKS_ITCPSERVERLISTENER_H
#define IDEA_NETWORKS_ITCPSERVERLISTENER_H

#include <string>

namespace idea::networks
{
	class ITCPServerListener {
	public:
		ITCPServerListener() = default;
		virtual ~ITCPServerListener() = default;

	public:
		virtual bool onTCPServerBound(std::size_t channel) = 0;
		virtual bool onReceivedTCPServerData(std::size_t channel, const std::string& data) = 0;
		virtual bool onReceivedTCPServerError(std::size_t channel, const std::string& data) = 0;
		virtual bool onTCPServerUnbound(std::size_t channel) = 0;
	};
}

#endif