#include <boost/log/trivial.hpp>
#include "TCPServer.h"
#include <memory>
#include <boost/asio.hpp>
#include "TCPSession.h"
#include "ITCPServerListener.h"

namespace idea::networks {
	TCPServer::TCPServer(boost::asio::io_context& ctx)
		: m_ctx(ctx)
		, m_acceptor(std::make_unique<boost::asio::ip::tcp::acceptor>(ctx))
		, m_sessions()
		, m_listeners()
		, m_mutex()
		, m_setStop(true)
	{}

	bool TCPServer::create(uint16_t port)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << port;
		try {
			m_acceptor->open(boost::asio::ip::tcp::v4());
			boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), port);
			m_acceptor->bind(ep);
			m_acceptor->listen();
		}
		catch (const std::exception& e) {
			BOOST_LOG_TRIVIAL(error) << e.what();
			return false;
		}

		return true;
	}

	bool TCPServer::bind()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		m_setStop = false;
		return accept();
	}

	bool TCPServer::write(std::size_t channel, const std::string& message)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		std::unique_lock<std::mutex> lock(m_mutex);
		if (m_sessions[channel] != nullptr) {
			m_sessions[channel]->write(message);
			return true;
		}
		else
			return false;
	}

	bool TCPServer::unbind()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		m_setStop = true;
		try {
			boost::asio::post(m_ctx, [this]() {
				m_acceptor->cancel();
			});
		}
		catch (const std::exception& e) {
			BOOST_LOG_TRIVIAL(error) << e.what();
			return false;
		}

		{
			std::unique_lock<std::mutex> lock(m_mutex);
			for (auto& session : m_sessions) {
				session.second->destroy();
				session.second->deleteListener(this);
			}
		}

		m_sessions.clear();
		return true;
	}

	bool TCPServer::destroy()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		try {
			boost::asio::post(m_ctx, [this]() {
				m_acceptor->close();
			});
		}
		catch (const std::exception& e) {
			BOOST_LOG_TRIVIAL(error) << e.what();
			return false;
		}

		return true;
	}

	bool TCPServer::onReceivedSessionRead(std::size_t channel, const std::string& message)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << channel << " " << message;
		for (auto listener : m_listeners)
			if (listener != nullptr)
				listener->onReceivedTCPServerData(channel, message);
		
		return true;
	}

	bool TCPServer::onReceivedSessionDisconnected(std::size_t channel)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << channel;
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			if (m_sessions[channel] != nullptr) {
				m_sessions[channel]->destroy();
				m_sessions[channel]->deleteListener(this);
				m_sessions.erase(channel);
			}
		}

		for (auto listener : m_listeners)
			if (listener != nullptr)
				listener->onTCPServerUnbound(channel);

		return true;
	}

	bool TCPServer::onReceivedSessionError(std::size_t channel, const std::string& error)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << channel << " " << error;
		for (auto listener : m_listeners)
			if (listener != nullptr)
				listener->onReceivedTCPServerError(channel, error);

		return true;
	}

	bool TCPServer::accept()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		m_acceptor->async_accept([this](const boost::system::error_code& ec, boost::asio::ip::tcp::socket socket) {
			if (!ec) {
				BOOST_LOG_TRIVIAL(info) << "Client connect!";
				auto session = std::make_shared<TCPSession>(m_sessions.size(), std::move(socket));
				session->create();
				session->addListener(this);
				{
					std::unique_lock<std::mutex> lock(m_mutex);
					m_sessions[m_sessions.size()] = session;
				}

				for (auto listener : m_listeners)
					if (listener != nullptr)
						listener->onTCPServerBound(m_sessions.size() - 1);

				if (!m_setStop)
					accept();
			}
			else {
				BOOST_LOG_TRIVIAL(error) << "Accept error: " << ec.message();
			}
		});
		return true;
	}
}