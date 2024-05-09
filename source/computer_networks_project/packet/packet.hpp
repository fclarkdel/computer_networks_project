#ifndef COMPUTER_NETWORKS_PROJECT_PACKET_HPP
#define COMPUTER_NETWORKS_PROJECT_PACKET_HPP

#include <string>
#include <sstream>
#include <optional>
#include <variant>

namespace computer_networks_project::packet {
	struct ethernet {
		static const std::size_t BROADCAST_ID = 99;

		std::size_t destination_id;
		std::size_t source_id;
		std::string data;

		bool operator==(const ethernet &rhs) const;
	};

	struct ip {
		std::size_t destination_network_id;
		std::size_t destination_host_id;
		std::size_t source_network_id;
		std::size_t source_host_id;
		std::string data;

		bool operator==(const ip &rhs) const;
	};

	enum class arp_types {
		REQ,
		REP
	};

	struct arp {
		arp_types arp_type;
		std::size_t target_network_id;
		std::size_t target_host_id;
		std::size_t target_ethernet_id;

		std::size_t source_network_id;
		std::size_t source_host_id;
		std::size_t source_ethernet_id;

		bool operator==(const arp &rhs) const;
	};

	struct hl {
		std::size_t network_id;
		std::size_t host_id;
		std::size_t ethernet_id;

		bool operator==(const hl &rhs) const;
	};

	struct bc {
		std::size_t source_network_id;
		std::size_t source_host_id;
		std::size_t sequence_number;
		std::size_t destination_network_id;
		std::string data;

		bool operator==(const bc &rhs) const;
	};

	using packet_types = std::variant<ethernet, ip, arp, hl, bc>;

	std::string serialize(const ethernet &packet);

	std::string serialize(const ip &packet);

	std::string serialize(const arp &packet);

	std::string serialize(const hl &packet);

	std::string serialize(const bc &packet);

	std::optional<packet_types> deserialize(const std::string &serialization);
}
#endif