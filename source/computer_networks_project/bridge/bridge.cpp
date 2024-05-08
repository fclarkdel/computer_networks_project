#include <bridge/bridge.hpp>

#include <iostream>

namespace computer_networks_project::bridge {
	bridge::bridge(
		std::size_t id,
		std::size_t number_of_ports,
		std::vector<std::size_t> adjacent_bridge_ids) :
		id{id} {
		for (std::size_t port_id = 1; port_id <= number_of_ports; ++port_id)
			links.emplace_back(link::link{id, port_id});

		std::size_t index = 0;

		for (const auto &adjacent_bridge_id: adjacent_bridge_ids) {
			links[index++] = link::bridge_link{id, adjacent_bridge_id};
		}
	}

	void bridge::process_packets() {
		for (auto &link: links) {
			while (std::optional<packet::packet_types> packet = link.read_bridge()) {
				std::visit(
					[&](auto &&packet) {
						// You have to decay the type to shed all the &, const, and what have you.
						using packet_type = std::decay_t<decltype(packet)>;

						if constexpr (std::is_same_v<packet_type, packet::ethernet>) {
							if (!ethernet_id_to_port_id.contains(packet.source_id))
								ethernet_id_to_port_id[packet.source_id] = link.port_id;

							if (packet.destination_id == packet::ethernet::BROADCAST_ID)
								// Using the broadcast id for the destination id sends the packet out over all ports
								// except the one it came through per the spec.
								broadcast(packet, link.port_id);
							else if (!ethernet_id_to_port_id.contains(packet.destination_id))
								// Packet destination id is unknown, so send it over all ports per the spec.
								broadcast(packet);
							else
								// This works because links are associated with their respective port_id in the vector
								// with the index as port_id - 1
								links[
									ethernet_id_to_port_id[
										packet.destination_id
									] - 1
								].write_bridge(packet);
						}
					},
					packet.value()
				);
			}
		}
	}

	void bridge::broadcast(auto packet, std::optional<std::size_t> exclude_port_id) {
		for (auto &link: links) {
			if (exclude_port_id.has_value() && link.port_id == exclude_port_id.value())
				continue;

			link.write_bridge(packet);
		}
	}
}