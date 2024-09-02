#ifndef IDEA_NETWORKS_CONTEXT_H
#define IDEA_NETWORKS_CONTEXT_H

#include <memory>
#include <vector>
#include <thread>

namespace boost::asio {
	class io_context;
}

namespace idea::networks {
	class Context
	{
	public:
		Context();
		Context(std::size_t threadNum);
		~Context();
		Context(const Context& context) = delete;
		Context(Context&& context) noexcept;
		Context& operator=(const Context& context) = delete;
		Context& operator=(Context&& context) noexcept;

	public:
		boost::asio::io_context& getContext();
		bool run();

	private:
		std::unique_ptr<boost::asio::io_context>							m_ctx;
		std::size_t															m_threadNum;
		std::vector<std::thread>											m_threads;
	};
}

#endif
