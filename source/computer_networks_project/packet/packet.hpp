#ifndef COMPUTER_NETWORKS_PROJECT_PACKET_HPP
#define COMPUTER_NETWORKS_PROJECT_PACKET_HPP

#include <string>
#include <sstream>
#include <optional>
#include <variant>

namespace computer_networks_project::packet {
	struct ethernet {
		std::size_t destination_id;
		std::size_t source_id;
		std::string data;

		bool operator==(const ethernet &rhs) const;
	};

	using packet_types = std::variant<ethernet>;

	std::string serialize(const ethernet &packet);

	std::optional<packet_types> deserialize(std::string serialization);
}
#endif