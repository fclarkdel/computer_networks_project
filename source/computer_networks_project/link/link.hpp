#ifndef COMPUTER_NETWORKS_PROJECT_LINK_HPP
#define COMPUTER_NETWORKS_PROJECT_LINK_HPP

#include <cstdint>
#include <fstream>
#include <optional>

#include <packet/packet.hpp>

namespace computer_networks_project::link {
	struct link {
		std::size_t bridge_id;
		std::size_t port_id;

		link(std::size_t bridge_id, std::size_t port_id);

		const std::string &get_from_filename() const;

		const std::string &get_to_filename() const;

		template<typename packet_type>
		void bridge_write(packet_type packet) {
			write_packet_to_stream(to_write_stream, packet);
		}

		template<typename packet_type>
		void port_write(packet_type packet) {
			write_packet_to_stream(from_write_stream, packet);
		}

		std::optional<packet::packet_types> bridge_read();

		std::optional<packet::packet_types> port_read();

	private:
		std::string from_filename;
		std::string to_filename;

		// Bridge reads from here
		// Port writes to here
		std::fstream from_read_stream;
		std::fstream from_write_stream;

		// Bridge writes to here
		// Port reads from here
		std::fstream to_read_stream;
		std::fstream to_write_stream;

		template<typename packet_type>
		static void write_packet_to_stream(std::fstream &stream, packet_type packet) {
			stream << packet::serialize(packet) << std::endl;
		}

		static std::optional<packet::packet_types> read_packet_from_stream(std::fstream &stream);
	};
}
#endif
