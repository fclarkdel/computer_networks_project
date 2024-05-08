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

		link(
			std::size_t bridge_id,
			std::size_t port_id
		);

		const std::string &get_from_filename() const;

		const std::string &get_to_filename() const;

		void write_bridge(auto packet) {
			write_packet_to_stream(to_write_stream, packet);
		}

		void write_port(auto packet) {
			write_packet_to_stream(from_write_stream, packet);
		}

		std::optional<packet::packet_types> read_bridge();

		std::optional<packet::packet_types> read_port();

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
	};

	struct bridge_link {
		std::size_t bridge_0_id;
		std::size_t bridge_1_id;

		bridge_link(
			std::size_t bridge_0_id,
			std::size_t bridge_1_id
		);

		const std::string &get_filename_0_to_1() const;

		const std::string &get_filename_1_to_0() const;

		void write_0_to_1(auto packet) {
			write_packet_to_stream(write_stream_0_to_1, packet);
		}

		void write_1_to_0(auto packet) {
			write_packet_to_stream(write_stream_1_to_0, packet);
		}

		std::optional<packet::packet_types> read_0_to_1();

		std::optional<packet::packet_types> read_1_to_0();

	private:
		std::string filename_0_to_1;
		std::string filename_1_to_0;

		// Bridge reads from here
		// Port writes to here
		std::fstream read_stream_0_to_1;
		std::fstream write_stream_0_to_1;

		// Bridge writes to here
		// Port reads from here
		std::fstream read_stream_1_to_0;
		std::fstream write_stream_1_to_0;
	};

	using link_types = std::variant<link, bridge_link>;

	void write_packet_to_stream(std::fstream &stream, auto packet) {
		stream << packet::serialize(packet) << std::endl;
	}

}
#endif
