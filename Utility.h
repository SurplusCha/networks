#ifndef IDEA_NETWORKS_UTILITY_H
#define IDEA_NETWORKS_UTILITY_H

#include <boost/asio.hpp>

namespace idea::networks {
	class Utility {
	public:
		~Utility() = default;

		static Utility& getInstance() {
			static Utility instance;
			return instance;
		}

	private:
		Utility() = default;
		Utility(const Utility& util) = delete;
		Utility(Utility&& util) noexcept = delete;
		Utility& operator=(const Utility& util) = delete;
		Utility& operator=(Utility&& util) noexcept = delete;
	};
}

#endif