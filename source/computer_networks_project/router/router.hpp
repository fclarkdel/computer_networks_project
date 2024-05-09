#ifndef COMPUTER_NETWORKS_PROJECT_ROUTER_HPP
#define COMPUTER_NETWORKS_PROJECT_ROUTER_HPP

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <link/link.hpp>

namespace computer_networks_project::router {
	struct router {
		router(
			std::vector<std::size_t> network_ids,
			std::vector<std::size_t> host_ids,
			std::vector<std::size_t> bridge_ids,
			std::vector<std::size_t> port_ids,
			std::vector<std::size_t> ethernet_ids
		);

		void hl_broadcast();

		void process_packets();

	private:
		std::vector<std::size_t> network_ids;
		std::vector<std::size_t> host_ids;
		std::vector<std::size_t> bridge_ids;
		std::vector<std::size_t> port_ids;
		std::vector<std::size_t> ethernet_ids;

		std::size_t bc_sequence_number;

		std::unordered_map<std::size_t, std::size_t> network_id_to_host_id;
		std::unordered_map<std::size_t, std::size_t> network_id_to_link_index;
		std::unordered_map<std::size_t, std::size_t> network_id_to_ethernet_id;
		std::unordered_map<std::size_t, std::unordered_set<std::size_t>> network_id_to_adjacent_router_host_ids;
		std::unordered_map<std::size_t, std::unordered_map<std::size_t, std::size_t>> ip_id_to_ethernet_id;
		std::unordered_map<std::size_t, std::unordered_map<std::size_t, std::vector<packet::ip>>> arp_buffer;
		std::unordered_map<std::size_t, std::unordered_map<std::size_t, std::size_t>> ip_id_to_bc_sequence_number;

		std::vector<link::link> links;

		void process_packet(const packet::ip &packet, std::size_t link_index);

		void process_packet(const packet::arp &packet, std::size_t link_index);

		void process_packet(const packet::hl &packet, std::size_t link_index);

		void process_packet(const packet::bc &packet, std::size_t link_index);

		void process_packet(packet::packet_types packet, std::size_t link_index);
	};
}
#endif
