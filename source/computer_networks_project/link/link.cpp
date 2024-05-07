#include <link/link.hpp>

namespace computer_networks_project::link {
	link::link(std::size_t bridge_id, std::size_t port_id) :
		bridge_id{bridge_id},
		port_id{port_id} {
		from_filename = std::string{"fromB" + std::to_string(bridge_id) + "P" + std::to_string(port_id) + ".txt"};
		to_filename = std::string{"toB" + std::to_string(bridge_id) + "P" + std::to_string(port_id) + ".txt"};
		// This block is necessary to create the files, without this weird behavior was occurring where the read and
		// write streams were not synchronized.
		{
			std::fstream{from_filename, std::fstream::in | std::fstream::app | std::fstream::out};
			std::fstream{to_filename, std::fstream::in | std::fstream::app | std::fstream::out};
		}
		// We need separate read and write streams because when they were combined, it would not read from the last
		// read position, it would read from where was last appended, so we'd always get nothing in.
		from_read_stream = std::fstream{from_filename, std::fstream::in};
		from_write_stream = std::fstream{from_filename, std::fstream::app};

		to_read_stream = std::fstream{to_filename, std::fstream::in};
		to_write_stream = std::fstream{to_filename, std::fstream::app};
	}

	std::optional<packet::packet_types> link::bridge_read() {
		return read_packet_from_stream(from_read_stream);
	}

	std::optional<packet::packet_types> link::port_read() {
		return read_packet_from_stream(to_read_stream);
	}

	std::optional<packet::packet_types> link::read_packet_from_stream(std::fstream &stream) {
		std::string serialized_packet{};

		std::getline(stream, serialized_packet);

		return packet::deserialize(serialized_packet);
	}

	const std::string &link::get_from_filename() const {
		return from_filename;
	}

	const std::string &link::get_to_filename() const {
		return to_filename;
	}
}