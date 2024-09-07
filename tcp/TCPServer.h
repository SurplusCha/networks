#ifndef IDEA_NETWORKS_TCPSERVER_H
#define IDEA_NETWORKS_TCPSERVER_H

#include <unordered_map>
#include <mutex>
#include <boost/asio.hpp>
#include "ISessionListener.h"

namespace idea::networks {
	class TCPSession;
	class ITCPServerListener;
	class TCPServer : public ISessionListener
	{
	public:
		TCPServer(boost::asio::io_context& ctx);
		virtual ~TCPServer() = default;
 		TCPServer(const TCPServer& server) = delete;
		TCPServer(TCPServer&& server) noexcept = delete;
		TCPServer& operator=(const TCPServer& server) = delete;
		TCPServer& operator=(TCPServer&& server) noexcept = delete;

	public:
		inline bool addListener(ITCPServerListener* listener) {
			if (std::find(std::begin(m_listeners), std::end(m_listeners), listener) == std::end(m_listeners)) {
				m_listeners.emplace_back(listener);
				return true;
			}
			return false;
		}
		inline bool deleteListsener(ITCPServerListener* listener) {
			if (std::find(std::begin(m_listeners), std::end(m_listeners), listener) != std::end(m_listeners))
				m_listeners.erase(std::remove(std::begin(m_listeners), std::end(m_listeners), listener), std::end(m_listeners));
			return true;
		}

	public:
		bool create(uint16_t port);
		bool bind();
		bool write(std::size_t channel, const std::string& message);
		bool unbind();
		bool destroy();

	public:
		bool onReceivedSessionRead(std::size_t channel, const std::string& message) override;
		bool onReceivedSessionDisconnected(std::size_t channel) override;
		bool onReceivedSessionError(std::size_t channel, const std::string& error) override;

	private:
		bool accept();

	private:
		boost::asio::io_context&										m_ctx;
		std::unique_ptr<boost::asio::ip::tcp::acceptor>					m_acceptor;
		std::unordered_map<std::size_t, std::shared_ptr<TCPSession>>	m_sessions;
		std::vector<ITCPServerListener*>								m_listeners;
		std::mutex														m_mutex;
		bool															m_setStop;
	};
}

#endif