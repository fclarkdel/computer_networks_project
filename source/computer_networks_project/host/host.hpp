#ifndef COMPUTER_NETWORKS_PROJECT_HOST_HPP
#define COMPUTER_NETWORKS_PROJECT_HOST_HPP

#include <unordered_map>
#include <vector>

#include <link/link.hpp>

namespace computer_networks_project::host {
	struct host {
		host(
			std::size_t network_id,
			std::size_t host_id,
			std::size_t bridge_id,
			std::size_t port_id,
			std::size_t ethernet_id
		);

		void process_packets();

	private:
		std::size_t network_id;
		std::size_t host_id;
		std::size_t bridge_id;
		std::size_t port_id;
		std::size_t ethernet_id;

		link::link link;

		std::unordered_map<std::size_t, std::unordered_map<std::size_t, std::size_t>> ip_id_to_ethernet_id;
		std::unordered_map<std::size_t, std::unordered_map<std::size_t, std::vector<packet::ip>>> arp_buffer;

		void process_packet(const packet::ip &packet);

		void process_packet(const packet::arp &packet);

		void process_packet(packet::packet_types packet);

	};
}

#endif
