#ifndef COMPUTER_NETWORKS_PROJECT_HOST_HPP
#define COMPUTER_NETWORKS_PROJECT_HOST_HPP

#include <array>
#include <queue>
#include <unordered_map>
#include <vector>
#include <iostream>

#include <link/link.hpp>

namespace computer_networks_project::host {

	struct host {
		host(
			std::size_t network_id,
			std::size_t host_id,
			std::size_t ethernet_id,
			std::size_t router_network_id,
			std::size_t router_host_id,
			std::size_t bridge_id,
			std::size_t port_id
		);

		void enqueue_data(std::size_t network_id, std::size_t host_id, const std::string &data);

		void process_packets();

	private:
		static const std::size_t NUMBER_OF_CHANNELS = 2;

		std::size_t network_id;
		std::size_t host_id;
		std::size_t ethernet_id;
		std::size_t router_network_id;
		std::size_t router_host_id;
		std::size_t bridge_id;
		std::size_t port_id;

		struct ip_data {
			std::size_t destination_network_id;
			std::size_t destination_host_id;
			std::string data;
		};
		std::queue<ip_data> data_queue;

		std::array<bool, NUMBER_OF_CHANNELS> sequence_bits{false, false};
		std::array<bool, NUMBER_OF_CHANNELS> ack_bits{false, false};

		link::link link;

		std::unordered_map<std::size_t, std::unordered_map<std::size_t, std::size_t>> ip_id_to_ethernet_id;
		std::unordered_map<std::size_t, std::unordered_map<std::size_t, std::vector<packet::ip>>> arp_buffer;

		void send_arp_request_for_and_buffer(const packet::ip &packet);

		void send_arp_request_to_and_buffer(std::size_t network_id, std::size_t host_id, const packet::ip &packet);

		void send_packet(const packet::ip &packet);

		void process_packet(const packet::ip &packet);

		void process_packet(const packet::arp &packet);

		void process_packet(packet::packet_types packet);
	};
}
#endif
