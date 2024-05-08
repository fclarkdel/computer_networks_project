#ifndef COMPUTER_NETWORKS_PROJECT_BRIDGE_HPP
#define COMPUTER_NETWORKS_PROJECT_BRIDGE_HPP

#include <cstdint>
#include <unordered_map>
#include <vector>

#include <link/link.hpp>

namespace computer_networks_project::bridge {
	struct bridge {
		bridge(
			std::size_t id,
			std::size_t number_of_ports,
			std::vector<std::size_t> adjacent_bridge_ids
		);

		void process_packets();

	private:
		std::size_t id;
		std::vector<link::link_types> links;
		std::unordered_map<std::size_t, std::size_t> ethernet_id_to_port_id;

		void broadcast(auto packet, std::optional<std::size_t> exclude_port_id = std::nullopt);
	};
}
#endif
