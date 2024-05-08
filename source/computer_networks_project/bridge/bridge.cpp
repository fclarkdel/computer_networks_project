#include <bridge/bridge.hpp>

#include <iostream>

namespace computer_networks_project::bridge {
	std::optional<packet::packet_types> read_link(bridge &reader, link::link &link) {
		return link.read_bridge();
	}

	std::optional<packet::packet_types> read_link(bridge &reader, link::bridge_link &link) {
		if (reader.get_id() == link.get_bridge_id_0())
			return link.read_1_to_0();
		else if (reader.get_id() == link.get_bridge_id_1())
			return link.read_0_to_1();
		else
			return std::nullopt;
	}

	std::optional<packet::packet_types> read_link(bridge &reader, link::link_types &link) {
		return std::visit(
			[&](auto &&link) {
				return read_link(reader, link);
			},
			link
		);
	}

	void write_link(bridge &writer, link::link &link, auto packet) {
		link.write_bridge(packet);
	}

	void write_link(bridge &writer, link::bridge_link &link, auto packet) {
		if (writer.get_id() == link.get_bridge_id_0())
			link.write_0_to_1(packet);
		else if (writer.get_id() == link.get_bridge_id_1())
			link.write_1_to_0(packet);
	}

	void write_link(bridge &writer, link::link_types &link, auto packet) {
		std::visit(
			[&](auto &&link) {
				write_link(writer, link, packet);
			},
			link
		);
	}

	bridge::bridge(
		std::size_t id,
		std::size_t number_of_ports,
		std::vector<std::size_t> adjacent_bridge_ids) :
		id{id} {
		for (std::size_t port_id = 1; port_id <= number_of_ports; ++port_id)
			links.emplace_back(link::link{id, port_id});

		for (const auto &adjacent_bridge_id: adjacent_bridge_ids) {
			links.emplace_back(link::bridge_link{id, adjacent_bridge_id});
		}
	}

	void bridge::process_packets() {
		for (std::size_t port_id = 1; port_id <= links.size(); ++port_id) {
			auto &link = links[port_id - 1];

			while (std::optional<packet::packet_types> packet = read_link(*this, link)) {
				std::visit(
					[&](auto &&packet) {
						// You have to decay the type to shed all the &, const, and what have you.
						using packet_type = std::decay_t<decltype(packet)>;

						if constexpr (std::is_same_v<packet_type, packet::ethernet>) {
							if (!ethernet_id_to_port_id.contains(packet.source_id))
								ethernet_id_to_port_id[packet.source_id] = port_id;

							if (packet.destination_id == packet::ethernet::BROADCAST_ID)
								// Using the broadcast id for the destination id sends the packet out over all ports
								// except the one it came through per the spec.
								broadcast(packet, port_id);
							else if (!ethernet_id_to_port_id.contains(packet.destination_id))
								// Packet destination id is unknown, so send it over all ports per the spec.
								broadcast(packet);
							else
								// This works because links are associated with their respective port_id in the vector
								// with the index as port_id - 1
								write_link(*this, links[ethernet_id_to_port_id[packet.destination_id] - 1], packet);
						}
					},
					packet.value()
				);
			}
		}
	}

	void bridge::broadcast(auto packet, std::optional<std::size_t> exclude_port_id) {
		for (std::size_t port_id = 1; port_id <= links.size(); ++port_id) {
			auto &link = links[port_id - 1];

			if (exclude_port_id.has_value() && port_id == exclude_port_id.value())
				continue;

			write_link(*this, link, packet);
		}
	}

	std::size_t bridge::get_id() const {
		return id;
	}
}
