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

	link.write_bridge(packet);

	std::fstream stream{link.get_to_filename()};
	std::string line;
	std::getline(stream, line);
	EXPECT_EQ(line, packet::serialize(packet));

	link.write_port(packet);

	stream.open(link.get_from_filename());
	std::getline(stream, line);
	EXPECT_EQ(line, packet::serialize(packet));

	link::bridge_link bridge_link{1, 2};
	bridge_link.write_0_to_1(packet);

	stream.close();
	stream.open(bridge_link.get_filename_0_to_1());
	std::getline(stream, line);
	EXPECT_EQ(line, packet::serialize(packet));

	bridge_link.write_1_to_0(packet);

	stream.close();
	stream.open(bridge_link.get_filename_1_to_0());
	std::getline(stream, line);
	EXPECT_EQ(line, packet::serialize(packet));
}

TEST_F(link_test, should_deserialize_packet_from_file) {
	packet::ethernet packet{1, 2, "data"};
	link::link link{1, 1};

	std::fstream stream{link.get_from_filename()};
	stream << packet::serialize(packet) << std::endl;

	packet::ethernet read_packet = std::get<packet::ethernet>(link.read_bridge().value());

	EXPECT_EQ(read_packet, packet);

	stream.close();
	stream.open(link.get_to_filename());
	stream << packet::serialize(packet) << std::endl;

	read_packet = std::get<packet::ethernet>(link.read_port().value());

	EXPECT_EQ(read_packet, packet);

	link::bridge_link bridge_link{1, 2};

	stream.close();
	stream.open(bridge_link.get_filename_0_to_1());
	stream << packet::serialize(packet) << std::endl;

	read_packet = std::get<packet::ethernet>(bridge_link.read_0_to_1().value());

	EXPECT_EQ(read_packet, packet);

	stream.close();
	stream.open(bridge_link.get_filename_1_to_0());
	stream << packet::serialize(packet) << std::endl;

	read_packet = std::get<packet::ethernet>(bridge_link.read_1_to_0().value());

	EXPECT_EQ(read_packet, packet);
}
