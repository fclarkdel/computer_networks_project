#include <packet/packet.hpp>

namespace computer_networks_project::packet {
	bool ethernet::operator==(const ethernet &rhs) const {
		return destination_id == rhs.destination_id &&
			   source_id == rhs.source_id &&
			   data == rhs.data;
	}

	bool ip::operator==(const ip &rhs) const {
		return destination_network_id == rhs.destination_network_id &&
			   destination_host_id == rhs.destination_host_id &&
			   source_network_id == rhs.source_network_id &&
			   source_host_id == rhs.source_host_id &&
			   data == rhs.data;
	}

	bool arp::operator==(const arp &rhs) const {
		return arp_type == rhs.arp_type &&
			   target_network_id == rhs.target_network_id &&
			   target_host_id == rhs.target_host_id &&
			   target_ethernet_id == rhs.target_ethernet_id &&
			   source_network_id == rhs.source_network_id &&
			   source_host_id == rhs.source_host_id &&
			   source_ethernet_id == rhs.source_ethernet_id;
	}

	bool hl::operator==(const hl &rhs) const {
		return network_id == rhs.network_id &&
			   host_id == rhs.host_id &&
			   ethernet_id == rhs.ethernet_id;
	}

	bool bc::operator==(const bc &rhs) const {
		return source_network_id == rhs.source_network_id &&
			   source_host_id == rhs.source_network_id &&
			   sequence_number == rhs.sequence_number &&
			   destination_network_id == rhs.destination_network_id &&
			   data == rhs.data;
	}

	std::string serialize(const ethernet &packet) {
		return "ethernet " +
			   std::to_string(packet.destination_id) + " " +
			   std::to_string(packet.source_id) + " " +
			   packet.data;
	}

	std::string serialize(const ip &packet) {
		return "ip " +
			   std::to_string(packet.destination_network_id) + " " +
			   std::to_string(packet.destination_host_id) + " " +
			   std::to_string(packet.source_network_id) + " " +
			   std::to_string(packet.source_host_id) + " " +
			   packet.data;
	}

	std::string serialize(const arp &packet) {
		std::string serialization = "arp ";

		switch (packet.arp_type) {
			case arp_types::REQ:
				serialization += "req ";
				break;
			case arp_types::REP:
				serialization += "rep ";
				break;
		}
		serialization += std::to_string(packet.target_network_id) + " ";
		serialization += std::to_string(packet.target_host_id) + " ";
		if (packet.arp_type == arp_types::REP)
			serialization += std::to_string(packet.target_ethernet_id) + " ";

		serialization += std::to_string(packet.source_network_id) + " ";
		serialization += std::to_string(packet.source_host_id) + " ";
		serialization += std::to_string(packet.source_ethernet_id);

		return serialization;
	}

	std::string serialize(const hl &packet) {
		return "hl " +
			   std::to_string(packet.network_id) + " " +
			   std::to_string(packet.host_id) + " " +
			   std::to_string(packet.ethernet_id);
	}

	std::string serialize(const bc &packet) {
		return "bc " +
			   std::to_string(packet.source_network_id) + " " +
			   std::to_string(packet.source_host_id) + " " +
			   std::to_string(packet.sequence_number) + " " +
			   std::to_string(packet.destination_network_id) + " " +
			   packet.data;
	}

	std::string serialize(const da &packet) {
		return "da " +
			   std::to_string(packet.sequence_number) + " " +
			   std::to_string(packet.channel_number) + " " +
			   packet.data;
	}

	std::string serialize(const ak &packet) {
		return "ak " +
			   std::to_string(packet.sequence_number) + " " +
			   std::to_string(packet.channel_number) + " ";
	}

	std::optional<packet_types> deserialize(const std::string &serialization) {
		std::istringstream stream{serialization};

		std::string packet_type{};
		std::getline(stream, packet_type, ' ');

		if (packet_type == "ethernet") {
			std::string destination_id{};
			std::string source_id{};
			std::string data{};

			std::getline(stream, destination_id, ' ');
			std::getline(stream, source_id, ' ');
			std::getline(stream, data);

			return ethernet{
				std::size_t{std::stoull(destination_id)},
				std::size_t{std::stoull(source_id)},
				data
			};
		} else if (packet_type == "ip") {
			std::string destination_network_id{};
			std::string destination_host_id{};
			std::string source_network_id{};
			std::string source_host_id{};
			std::string data{};

			std::getline(stream, destination_network_id, ' ');
			std::getline(stream, destination_host_id, ' ');
			std::getline(stream, source_network_id, ' ');
			std::getline(stream, source_host_id, ' ');
			std::getline(stream, data);

			return ip{
				std::size_t{std::stoull(destination_network_id)},
				std::size_t{std::stoull(destination_host_id)},
				std::size_t{std::stoull(source_network_id)},
				std::size_t{std::stoull(source_host_id)},
				data
			};
		} else if (packet_type == "arp") {
			std::string arp_type{};
			std::string target_network_id;
			std::string target_host_id;
			std::string target_ethernet_id;
			std::string source_network_id;
			std::string source_host_id;
			std::string source_ethernet_id;

			std::getline(stream, arp_type, ' ');
			std::getline(stream, target_network_id, ' ');
			std::getline(stream, target_host_id, ' ');

			if (arp_type == "rep")
				std::getline(stream, target_ethernet_id, ' ');

			std::getline(stream, source_network_id, ' ');
			std::getline(stream, source_host_id, ' ');
			std::getline(stream, source_ethernet_id, ' ');

			return arp{
				arp_type == "req" ? arp_types::REQ : arp_types::REP,
				std::size_t{std::stoull(target_network_id)},
				std::size_t{std::stoull(target_host_id)},
				arp_type == "req" ? 0 : std::size_t{std::stoull(target_ethernet_id)},
				std::size_t{std::stoull(source_network_id)},
				std::size_t{std::stoull(source_host_id)},
				std::size_t{std::stoull(source_ethernet_id)}
			};
		} else if (packet_type == "hl") {
			std::string network_id{};
			std::string host_id{};
			std::string ethernet_id{};

			std::getline(stream, network_id, ' ');
			std::getline(stream, host_id, ' ');
			std::getline(stream, ethernet_id, ' ');

			return hl{
				std::size_t{std::stoull(network_id)},
				std::size_t{std::stoull(host_id)},
				std::size_t{std::stoull(ethernet_id)}
			};
		} else if (packet_type == "bc") {
			std::string source_network_id{};
			std::string source_host_id{};
			std::string sequence_number{};
			std::string destination_network_id{};
			std::string data{};

			std::getline(stream, source_network_id, ' ');
			std::getline(stream, source_host_id, ' ');
			std::getline(stream, sequence_number, ' ');
			std::getline(stream, destination_network_id, ' ');
			std::getline(stream, data);

			return bc{
				std::size_t{std::stoull(source_network_id)},
				std::size_t{std::stoull(source_host_id)},
				std::size_t{std::stoull(sequence_number)},
				std::size_t{std::stoull(destination_network_id)},
				data
			};
		} else if (packet_type == "da") {
			std::string sequence_number{};
			std::string channel_number{};
			std::string data;

			std::getline(stream, sequence_number, ' ');
			std::getline(stream, channel_number, ' ');
			std::getline(stream, data);

			return da{
				std::size_t{std::stoull(sequence_number)},
				std::size_t{std::stoull(channel_number)},
				data
			};
		} else if (packet_type == "ak") {
			std::string sequence_number{};
			std::string channel_number{};

			std::getline(stream, sequence_number, ' ');
			std::getline(stream, channel_number, ' ');

			return ak{
				std::size_t{std::stoull(sequence_number)},
				std::size_t{std::stoull(channel_number)},
			};
		}
		return std::nullopt;
	}
}
