#include <link/link.hpp>

namespace computer_networks_project::link {
	std::optional<packet::packet_types> read_packet_from_stream(std::fstream &stream) {
		std::string serialized_packet{};

		stream.clear();
		std::getline(stream, serialized_packet);

		return packet::deserialize(serialized_packet);
	}

	link::link(
		std::size_t bridge_id,
		std::size_t port_id) :
		bridge_id{bridge_id},
		port_id{port_id} {
		from_filename = std::string{"fromB" + std::to_string(bridge_id) + "P" + std::to_string(port_id) + ".txt"};
		to_filename = std::string{"toB" + std::to_string(bridge_id) + "P" + std::to_string(port_id) + ".txt"};
		// This block is necessary to create the files, without this weird behavior was occurring where the read and
		// write streams were not synchronized.
		{
			std::fstream{from_filename, std::fstream::app};
			std::fstream{to_filename, std::fstream::app};
		}
		// We need separate read and write streams because when they were combined, it would not read from the last
		// read position, it would read from where was last appended, so we'd always get nothing in.
		from_read_stream = std::fstream{from_filename, std::fstream::in};
		from_write_stream = std::fstream{from_filename, std::fstream::app};

		to_read_stream = std::fstream{to_filename, std::fstream::in};
		to_write_stream = std::fstream{to_filename, std::fstream::app};
	}

	std::optional<packet::packet_types> link::read_bridge() {
		return read_packet_from_stream(from_read_stream);
	}

	std::optional<packet::packet_types> link::read_port() {
		return read_packet_from_stream(to_read_stream);
	}


	const std::string &link::get_from_filename() const {
		return from_filename;
	}

	const std::string &link::get_to_filename() const {
		return to_filename;
	}

	bridge_link::bridge_link(
		std::size_t bridge_0_id,
		std::size_t bridge_1_id) :
		bridge_0_id{bridge_0_id},
		bridge_1_id{bridge_1_id} {
		filename_0_to_1 = std::string{"B" + std::to_string(bridge_0_id) + "B" + std::to_string(bridge_1_id) + ".txt"};
		filename_1_to_0 = std::string{"B" + std::to_string(bridge_1_id) + "B" + std::to_string(bridge_0_id) + ".txt"};
		// This block is necessary to create the files, without this weird behavior was occurring where the read and
		// write streams were not synchronized.
		{
			std::fstream{filename_0_to_1, std::fstream::app};
			std::fstream{filename_1_to_0, std::fstream::app};
		}
		// We need separate read and write streams because when they were combined, it would not read from the last
		// read position, it would read from where was last appended, so we'd always get nothing in.
		read_stream_0_to_1 = std::fstream{filename_0_to_1, std::fstream::in};
		write_stream_0_to_1 = std::fstream{filename_0_to_1, std::fstream::app};

		read_stream_1_to_0 = std::fstream{filename_1_to_0, std::fstream::in};
		write_stream_1_to_0 = std::fstream{filename_1_to_0, std::fstream::app};
	}

	const std::string &bridge_link::get_filename_0_to_1() const {
		return filename_0_to_1;
	}

	const std::string &bridge_link::get_filename_1_to_0() const {
		return filename_1_to_0;
	}

	std::optional<packet::packet_types> bridge_link::read_0_to_1() {
		return read_packet_from_stream(read_stream_0_to_1);
	}

	std::optional<packet::packet_types> bridge_link::read_1_to_0() {
		return read_packet_from_stream(read_stream_1_to_0);
	}
}