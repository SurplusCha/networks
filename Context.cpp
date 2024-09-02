#include "Context.h"
#include <boost/asio.hpp>

namespace idea::networks {
	Context::Context()
		: m_ctx(std::make_unique<boost::asio::io_context>())
		, m_threadNum(std::thread::hardware_concurrency())
		, m_threads()
	{
		m_threads.reserve(m_threadNum);
	}

	Context::Context(std::size_t threadNum)
		: m_ctx(std::make_unique<boost::asio::io_context>())
		, m_threadNum(threadNum)
		, m_threads()
	{
		m_threads.reserve(threadNum);
	}

	Context::~Context()
	{
		for (auto& thread : m_threads)
			if (thread.joinable())
				thread.join();
	}

	Context::Context(Context&& context) noexcept
		: m_ctx(std::move(context.m_ctx))
		, m_threadNum(context.m_threadNum)
		, m_threads(std::move(context.m_threadNum))
	{}
	
	Context& Context::operator=(Context&& context) noexcept
	{
		if (this != &context) {
			m_ctx = std::move(context.m_ctx);
		}

		return *this;
	}

	boost::asio::io_context& Context::getContext()
	{
		return *m_ctx;
	}

	bool Context::run()
	{
		for (auto i = 0; i < m_threadNum; ++i) {
			m_threads.emplace_back([this]() {
				m_ctx->run();
			});
		}

		return true;
	}
}