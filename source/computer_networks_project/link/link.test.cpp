#include <gtest/gtest.h>

#include <link/link.hpp>
#include <utils/utils.hpp>

using namespace computer_networks_project;

struct link_test : public ::testing::Test {
	~link_test() override {
		utils::remove_link_files_in_current_directory();
	}
};

TEST_F(link_test, should_write_serialized_packet_to_file) {
	packet::ethernet packet{1, 2, "data"};

	link::link link{1, 1};

	link.bridge_write(packet);

	std::fstream stream{link.get_to_filename()};
	std::string line;
	std::getline(stream, line);
	EXPECT_EQ(line, packet::serialize(packet));

	link.port_write(packet);

	stream.open(link.get_from_filename());
	std::getline(stream, line);
	EXPECT_EQ(line, packet::serialize(packet));
}

TEST_F(link_test, should_deserialize_packet_from_file) {
	packet::ethernet packet{1, 2, "data"};

	link::link link{1, 1};

	std::fstream stream{link.get_from_filename()};

	stream << packet::serialize(packet) << std::endl;

	packet::ethernet read_packet = std::get<packet::ethernet>(link.bridge_read().value());

	EXPECT_EQ(read_packet, packet);
}
