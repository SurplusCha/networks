#ifndef IDEA_NETWORKS_TCP_ISESSIONLISTENER_H
#define IDEA_NETWORKS_TCP_ISESSIONLISTENER_H

#include <string>

namespace idea::networks::tcp
{
	class ISessionListener {
	public:
		ISessionListener() = default;
		virtual ~ISessionListener() = default;

	public:
		virtual bool onReceivedSessionRead(std::size_t channel, const std::string& message) = 0;
		virtual bool onReceivedSessionDisconnected(std::size_t channel) = 0;
		virtual bool onReceivedSessionError(std::size_t channel, const std::string& error) = 0;
	};
}

#endif