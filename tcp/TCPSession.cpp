#include <boost/log/trivial.hpp>
#include <source_location>
#include <span>
#include "TCPSession.h"
#include "ISessionListener.h"

namespace idea::networks::tcp {
	TCPSession::TCPSession(std::size_t channel, boost::asio::ip::tcp::socket&& socket)
		: m_channel(channel)
		, m_socket(std::move(socket))
		, m_strand(boost::asio::make_strand(socket.get_executor()))
		, m_data(1024)
		, m_writeBuffer()
		, m_listeners()
	{}
	TCPSession::TCPSession(TCPSession&& session) noexcept
		: m_channel(session.m_channel)
		, m_socket(std::move(session.m_socket))
		, m_strand(std::move(session.m_strand))
		, m_data(std::move(session.m_data))
		, m_writeBuffer(std::move(session.m_writeBuffer))
		, m_listeners(std::move(session.m_listeners))
	{}
	TCPSession& TCPSession::operator=(TCPSession&& session) noexcept
	{
		if (this != &session)
		{
			m_channel = session.m_channel;
			m_socket = std::move(session.m_socket);
			m_strand = std::move(session.m_strand);
			m_data = std::move(session.m_data);
			m_writeBuffer = std::move(session.m_writeBuffer);
			m_listeners = std::move(session.m_listeners);
		}

		return *this;
	}

	bool TCPSession::create()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		return onRead();
	}

	bool TCPSession::write(const std::string& message)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name() << message;
		auto self(shared_from_this());
		boost::asio::post(m_strand, [this, self, message]() {
			bool writeInProgress = !m_writeBuffer.empty();
			m_writeBuffer.emplace_back(message);
			if (!writeInProgress)
				onWrite();
			});
		return true;
	}

	bool TCPSession::destroy()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		try {
			m_socket.cancel();
			m_socket.close();
		}
		catch (const std::exception& e) {
			BOOST_LOG_TRIVIAL(error) << e.what();
			return false;
		}

		return true;
	}

	bool TCPSession::addListener(ISessionListener* listener)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		if (std::find(std::begin(m_listeners), std::end(m_listeners), listener) == std::end(m_listeners)) {
			m_listeners.emplace_back(listener);
			return true;
		}
		return false;
	}

	bool TCPSession::deleteListener(ISessionListener* listener)
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		if (std::find(std::begin(m_listeners), std::end(m_listeners), listener) != std::end(m_listeners)) {
			m_listeners.erase(std::remove(std::begin(m_listeners), std::end(m_listeners), listener), std::end(m_listeners));
		}
		return true;
	}

	bool TCPSession::onRead()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		auto self(shared_from_this());
		m_socket.async_read_some(boost::asio::buffer(m_data),
			boost::asio::bind_executor(m_strand, [this, self](const boost::system::error_code& ec, std::size_t length) {
				if (!ec) {
					for (auto listener : m_listeners) {
						if (listener != nullptr) {
							auto data = std::span<char>(m_data.data(), length);
							listener->onReceivedSessionRead(m_channel, std::string(std::begin(data), std::end(data)));
						}
					}

					onRead();
				}
				else if (ec == boost::asio::error::eof) {
					for (auto listener : m_listeners)
						if (listener != nullptr)
							listener->onReceivedSessionDisconnected(m_channel);
				}
				else {
					for (auto listener : m_listeners)
						if (listener != nullptr)
							listener->onReceivedSessionError(m_channel, ec.message());
				}
			}));
		return true;
	}

	bool TCPSession::onWrite()
	{
		BOOST_LOG_TRIVIAL(trace) << std::source_location::current().function_name();
		auto self(shared_from_this());
		boost::asio::async_write(m_socket, boost::asio::buffer(m_writeBuffer.front()),
			boost::asio::bind_executor(m_strand, [this, self](const boost::system::error_code& ec, std::size_t) {
				if (!ec) {
					BOOST_LOG_TRIVIAL(trace) << "Send message from Session.";
					m_writeBuffer.pop_front();
					if (!m_writeBuffer.empty())
						onWrite();
				}
				else {
					m_writeBuffer.clear();
					for (auto listener : m_listeners)
						if (listener != nullptr)
							listener->onReceivedSessionError(m_channel, ec.message());
				}
			}));
		return true;
	}
}