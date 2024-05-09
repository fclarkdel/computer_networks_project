#include <router/router.hpp>

namespace computer_networks_project::router {
	router::router(
		std::vector<std::size_t> network_ids,
		std::vector<std::size_t> host_ids,
		std::vector<std::size_t> bridge_ids,
		std::vector<std::size_t> port_ids,
		std::vector<std::size_t> ethernet_ids
	) :
		network_ids{network_ids},
		host_ids{host_ids},
		bridge_ids{bridge_ids},
		port_ids{port_ids},
		ethernet_ids{ethernet_ids} {
		for (std::size_t index = 0; index < bridge_ids.size(); ++index) {
			links.emplace_back(bridge_ids[index], port_ids[index]);
		}
	}

	void router::hello_broadcast() {
		for (std::size_t index = 0; index < links.size(); ++index) {
			packet::ethernet ethernet_packet{
				packet::ethernet::BROADCAST_ID,
				ethernet_ids[index]
			};
			packet::hl hl_packet{
				network_ids[index],
				host_ids[index],
				ethernet_ids[index]
			};
			ethernet_packet.data = packet::serialize(hl_packet);

			links[index].write_port(ethernet_packet);
		}
	}

	void router::process_packet(const packet::ip &packet, std::size_t link_index) {
		if (packet.destination_network_id == network_ids[link_index] &&
			packet.destination_host_id == host_ids[link_index]) {
			// Do not forward this packet since it is addressed for this router.

			return;
		} else if (ip_id_to_ethernet_id.contains(packet.destination_network_id) &&
				   ip_id_to_ethernet_id[packet.destination_network_id].contains(packet.destination_host_id)) {
			// Do know ethernet id for this ip id.

			links[link_index].write_port(
				packet::ethernet{
					ip_id_to_ethernet_id[packet.destination_network_id][packet.destination_host_id],
					ethernet_ids[link_index],
					packet::serialize(packet)
				}
			);
		} else {
			// Do not know ethernet id for this ip id, broadcast an ARP REQ and buffer IP packet.
			if (!arp_buffer.contains(packet.destination_network_id))
				arp_buffer[packet.destination_network_id] = {};

			if (!arp_buffer[packet.destination_network_id].contains(packet.destination_host_id))
				arp_buffer[packet.destination_network_id][packet.destination_host_id] = {};

			arp_buffer[packet.destination_network_id][packet.destination_host_id].emplace_back(packet);

			packet::arp arp_packet{
				packet::arp_types::REQ,
				packet.destination_network_id,
				packet.destination_host_id,
				0,
				network_ids[link_index],
				host_ids[link_index],
				ethernet_ids[link_index]
			};
			packet::ethernet ethernet_packet{
				packet::ethernet::BROADCAST_ID,
				ethernet_ids[link_index],
				packet::serialize(arp_packet)
			};
			links[link_index].write_port(
				ethernet_packet
			);
		}
	}

	void router::process_packet(const packet::arp &packet, std::size_t link_index) {
		if (packet.arp_type == packet::arp_types::REQ &&
			packet.target_network_id == network_ids[link_index] &&
			packet.target_host_id == host_ids[link_index]) {
			// ARP REQ directed to this router, reply with ARP REP.

			packet::arp arp_packet{
				packet::arp_types::REP,
				network_ids[link_index],
				host_ids[link_index],
				ethernet_ids[link_index],
				packet.source_network_id,
				packet.source_host_id,
				packet.source_ethernet_id
			};
			packet::ethernet ethernet_packet{
				packet.source_ethernet_id,
				ethernet_ids[link_index],
				packet::serialize(arp_packet)
			};
			links[link_index].write_port(
				ethernet_packet
			);
		} else if (packet.source_network_id == network_ids[link_index] &&
				   packet.source_host_id == host_ids[link_index]) {
			// ARP REP directed to this router.
			if (!ip_id_to_ethernet_id.contains(packet.target_network_id))
				ip_id_to_ethernet_id[packet.target_network_id] = {};

			ip_id_to_ethernet_id[packet.target_network_id][packet.target_host_id] = packet.target_ethernet_id;

			for (auto &&ip_packet: arp_buffer[packet.target_network_id][packet.target_host_id]) {
				links[link_index].write_port(
					packet::ethernet{
						packet.target_ethernet_id,
						ethernet_ids[link_index],
						packet::serialize(ip_packet)
					}
				);
			}
			arp_buffer[packet.target_network_id][packet.target_host_id].clear();
		}
	}

	void router::process_packet(const packet::hl &packet, std::size_t link_index) {
		if (packet.network_id == network_ids[link_index] &&
			packet.host_id == host_ids[link_index] &&
			packet.ethernet_id == ethernet_ids[link_index])
			return;

		if (!network_id_to_adjacent_router_host_ids.contains(packet.network_id))
			network_id_to_adjacent_router_host_ids[packet.network_id] = {};

		network_id_to_adjacent_router_host_ids[packet.network_id].insert(packet.host_id);
	}

	void router::process_packet(packet::packet_types packet, std::size_t link_index) {
		packet::ethernet ethernet_packet = std::get<packet::ethernet>(packet);

		// If this packet is not meant for us, do nothing with it.
		if (ethernet_packet.destination_id != packet::ethernet::BROADCAST_ID &&
			ethernet_packet.destination_id != ethernet_ids[link_index])
			return;

		std::visit(
			[&](auto &&packet) {
				process_packet(packet, link_index);
			},
			// Shed the ethernet shell and process what is encapsulated.
			packet::deserialize(
				ethernet_packet.data
			).value()
		);
	}

	void router::process_packets() {
		for (std::size_t link_index = 0; link_index < links.size(); ++link_index) {
			while (auto packet = links[link_index].read_port())
				process_packet(packet.value(), link_index);
		}
	}
}
