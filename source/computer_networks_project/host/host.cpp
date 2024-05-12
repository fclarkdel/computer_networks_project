#include <host/host.hpp>

namespace computer_networks_project::host {

	host::host(
		std::size_t network_id,
		std::size_t host_id,
		std::size_t ethernet_id,
		std::size_t router_network_id,
		std::size_t router_host_id,
		std::size_t bridge_id,
		std::size_t port_id) :
		network_id{network_id},
		host_id{host_id},
		ethernet_id{ethernet_id},
		router_network_id{router_network_id},
		router_host_id{router_host_id},
		bridge_id{bridge_id},
		port_id{port_id},
		link{bridge_id, port_id} {
	}

	void host::send_packet(const packet::ip &packet) {
		// In this network?
		if (packet.destination_network_id == network_id) {
			// Know associated ethernet id?
			if (ip_id_to_ethernet_id.contains(packet.destination_network_id) &&
				ip_id_to_ethernet_id[packet.destination_network_id].contains(packet.destination_host_id)) {

				// Write the packet to that link.
				link.write_port(
					packet::ethernet{
						ip_id_to_ethernet_id[packet.destination_network_id][packet.destination_host_id],
						ethernet_id,
						packet::serialize(packet)
					}
				);
			} else {
				send_arp_request_for_and_buffer(packet);
			}
		} else {
			// Know router ethernet id?
			if (ip_id_to_ethernet_id.contains(router_network_id) &&
				ip_id_to_ethernet_id[router_network_id].contains(router_host_id)) {

				// Write the packet to the default router.
				link.write_port(
					packet::ethernet{
						ip_id_to_ethernet_id[router_network_id][router_host_id],
						ethernet_id,
						packet::serialize(packet)
					}
				);
			} else {
				send_arp_request_to_and_buffer(router_network_id, router_host_id, packet);
			}
		}
	}

	void
	host::send_arp_request_to_and_buffer(std::size_t target_network_id, std::size_t target_host_id,
										 const packet::ip &packet) {
		// Buffer the packet.
		if (!arp_buffer.contains(packet.destination_network_id))
			arp_buffer[packet.destination_network_id] = {};

		if (!arp_buffer[packet.destination_network_id].contains(packet.destination_host_id))
			arp_buffer[packet.destination_network_id][packet.destination_host_id] = {};

		arp_buffer[target_network_id][target_host_id].emplace_back(
			packet);

		// Broadcast ARP REQ over all links.
		packet::arp arp_packet{
			packet::arp_types::REQ,
			target_network_id,
			target_host_id,
			0,
			network_id,
			host_id,
			ethernet_id
		};
		packet::ethernet ethernet_packet{
			packet::ethernet::BROADCAST_ID,
			ethernet_id,
			packet::serialize(arp_packet)
		};
		link.write_port(ethernet_packet);
	}

	void host::send_arp_request_for_and_buffer(const packet::ip &packet) {
		send_arp_request_to_and_buffer(packet.destination_network_id, packet.destination_host_id, packet);
	}

	void host::process_packet(const packet::ip &packet) {
		if (packet.destination_network_id != network_id || packet.destination_host_id != host_id)
			return;

		auto encapsulated_packet = packet::deserialize(packet.data);

		if (!encapsulated_packet.has_value())
			return;

		std::visit(
			[&, this](auto &&_packet) {
				using packet_type = std::decay_t<decltype(_packet)>;

				if constexpr (std::is_same_v<packet_type, packet::da>) {
					std::cout << packet::serialize(_packet) << std::endl;

					packet::ak ak_packet{
						_packet.sequence_number,
						_packet.channel_number
					};
					packet::ip ip_packet{
						packet.source_network_id,
						packet.source_host_id,
						network_id,
						host_id,
						packet::serialize(ak_packet)
					};
					send_packet(ip_packet);
				} else if constexpr (std::is_same_v<packet_type, packet::ak>) {
					std::cout << packet::serialize(_packet) << std::endl;

					ack_bits[_packet.channel_number] = _packet.sequence_number;
				}
			},
			encapsulated_packet.value()
		);

	}

	void host::process_packet(const packet::arp &packet) {
		if (packet.arp_type == packet::arp_types::REQ &&
			packet.target_network_id == network_id &&
			packet.target_host_id == host_id) {
			// ARP REQ directed to this host, reply with ARP REP.

			packet::arp arp_packet{
				packet::arp_types::REP,
				network_id,
				host_id,
				ethernet_id,
				packet.source_network_id,
				packet.source_host_id,
				packet.source_ethernet_id
			};
			packet::ethernet ethernet_packet{
				packet.source_ethernet_id,
				ethernet_id,
				packet::serialize(arp_packet)
			};
			link.write_port(
				ethernet_packet
			);
		} else if (packet.source_network_id == network_id &&
				   packet.source_host_id == host_id) {
			// ARP REP directed to this host.
			if (!ip_id_to_ethernet_id.contains(packet.target_network_id))
				ip_id_to_ethernet_id[packet.target_network_id] = {};

			ip_id_to_ethernet_id[packet.target_network_id][packet.target_host_id] = packet.target_ethernet_id;

			for (auto &&ip_packet: arp_buffer[packet.target_network_id][packet.target_host_id]) {
				link.write_port(
					packet::ethernet{
						packet.target_ethernet_id,
						ethernet_id,
						packet::serialize(ip_packet)
					}
				);
			}
			arp_buffer[packet.target_network_id][packet.target_host_id].clear();
		}
	}

	void host::process_packet(const packet::hl &packet) {

	}

	void host::process_packet(packet::packet_types packet) {
		packet::ethernet ethernet_packet = std::get<packet::ethernet>(packet);

		// If this packet is not meant for us, do nothing with it.
		if (ethernet_packet.destination_id != packet::ethernet::BROADCAST_ID &&
			ethernet_packet.destination_id != ethernet_id)
			return;

		std::visit(
			[&](auto &&packet) {
				process_packet(packet);
			},
			// Shed the ethernet shell and process what is encapsulated.
			packet::deserialize(
				ethernet_packet.data
			).value()
		);
	}

	void host::process_packets() {
		while (auto packet = link.read_port())
			process_packet(packet.value());

		for (std::size_t index = 0; index < NUMBER_OF_CHANNELS && !data_queue.empty(); ++index) {
			if (sequence_bits[index] != ack_bits[index])
				continue;

			ip_data data = data_queue.front();
			data_queue.pop();

			sequence_bits[index] = !sequence_bits[index];

			send_packet(
				packet::ip{
					data.destination_network_id,
					data.destination_host_id,
					network_id,
					host_id,
					packet::serialize(
						packet::da{
							sequence_bits[index],
							index,
							data.data
						}
					)
				}
			);
		}

	}

	void
	host::enqueue_data(std::size_t destination_network_id, std::size_t destination_host_id,
					   const std::string &data) {
		// Split data into 5 byte data packets.
		for (std::size_t index = 0; index < data.size(); index += 5)
			data_queue.emplace(destination_network_id, destination_host_id, data.substr(index, 5));
	}
}
