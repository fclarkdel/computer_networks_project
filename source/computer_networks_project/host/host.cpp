#include <host/host.hpp>

namespace computer_networks_project::host {

	host::host(
		std::size_t network_id,
		std::size_t host_id,
		std::size_t bridge_id,
		std::size_t port_id,
		std::size_t ethernet_id) :
		network_id{network_id},
		host_id{host_id},
		bridge_id{bridge_id},
		port_id{port_id},
		ethernet_id{ethernet_id},
		link{bridge_id, port_id} {
	}

	void host::process_packet(const packet::ip &packet) {
		if (packet.destination_network_id == network_id &&
			packet.destination_host_id == host_id) {

			return;
		} else if (ip_id_to_ethernet_id.contains(packet.destination_network_id) &&
				   ip_id_to_ethernet_id[packet.destination_network_id].contains(packet.destination_host_id)) {
			// Do know ethernet id for this ip id.

			link.write_port(
				packet::ethernet{
					ip_id_to_ethernet_id[packet.destination_network_id][packet.destination_host_id],
					ethernet_id,
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
				network_id,
				host_id,
				ethernet_id
			};
			packet::ethernet ethernet_packet{
				packet::ethernet::BROADCAST_ID,
				ethernet_id,
				packet::serialize(arp_packet)
			};
			link.write_port(
				ethernet_packet
			);
		}
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
	}
}
