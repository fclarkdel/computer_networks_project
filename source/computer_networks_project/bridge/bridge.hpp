#ifndef COMPUTER_NETWORKS_PROJECT_BRIDGE_HPP
#define COMPUTER_NETWORKS_PROJECT_BRIDGE_HPP

#include <cstdint>

namespace computer_networks_project::bridge {
	struct bridge {
		bridge(
			std::size_t id,
			std::size_t number_of_ports,
			std::vector<std::size_t> neighboring_bridge_ids
		);

	private:
		std::size_t id;

	};
}
#endif
