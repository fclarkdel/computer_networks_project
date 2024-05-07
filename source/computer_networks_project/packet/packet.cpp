#include <packet/packet.hpp>

namespace computer_networks_project::packet {
	bool ethernet::operator==(const ethernet &rhs) const {
		return destination_id == rhs.destination_id &&
			   source_id == rhs.source_id &&
			   data == rhs.data;
	}

	std::string serialize(const ethernet &packet) {
		return "ethernet " +
			   std::to_string(packet.destination_id) +
			   " " +
			   std::to_string(packet.source_id) +
			   " " +
			   packet.data;
	}

	using packet_types = std::variant<ethernet>;

	std::optional<packet_types> deserialize(std::string serialization) {
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
		} else
			return std::nullopt;
	}
}