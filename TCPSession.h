#ifndef IDEA_NETWORKS_TCPSESSION_H
#define IDEA_NETWORKS_TCPSESSION_H

#include <deque>
#include <boost/asio.hpp>

namespace idea::networks
{
	class ISessionListener;
	class TCPSession : public std::enable_shared_from_this<TCPSession>
	{
	public:
		TCPSession() = default;
		TCPSession(std::size_t channel, boost::asio::ip::tcp::socket&& socket);
		virtual ~TCPSession() = default;
		TCPSession(const TCPSession& session) = delete;
		TCPSession(TCPSession&& session) noexcept;
		TCPSession& operator=(const TCPSession& session) = delete;
		TCPSession& operator=(TCPSession&& session) noexcept;

	public:
		bool create();
		bool write(const std::string& message);
		inline std::size_t getChannel()										{ return m_channel; }
		bool destroy();

	public:
		bool addListener(ISessionListener* listener);
		bool deleteListener(ISessionListener* listener);

	private:
		bool onRead();
		bool onWrite();

	private:
		std::size_t															m_channel;
		boost::asio::ip::tcp::socket										m_socket;
		boost::asio::strand<boost::asio::any_io_executor>					m_strand;
		std::vector<char>													m_data;
		std::deque<std::string>												m_writeBuffer;

	private:
		std::vector<ISessionListener*>										m_listeners;
	};
}
#endif