#include <gtest/gtest.h>

#include <bridge/bridge.hpp>
#include <packet/packet.hpp>
#include <utils/utils.hpp>

using namespace computer_networks_project;

struct bridge_test : public ::testing::Test {
	~bridge_test() override {
		utils::remove_link_files_in_current_directory();
	}
};

TEST_F(bridge_test, dev) {
//	packet::ethernet packet{99, 1, "data"};
//
//	link::link link11{1, 1};
	// Something about reading pushes the stream into an invalid state where it is no longer capable of reading future
	// writes.
	//
	// I think it may have to do with reading the final entry in a file, because checking for EOF does not seem to help.
//	auto temp0 = link11.bridge_read();
//	link11.port_write(packet);
//	auto temp1 = link11.bridge_read();

	{
		std::fstream{"test.txt", std::fstream::app};
	}
	std::fstream read_stream{"test.txt", std::fstream::in};
	std::fstream write_stream{"test.txt", std::fstream::app};

//	write_stream << std::endl;

	write_stream << packet::serialize(packet::ethernet{1, 2, "data"}) << std::endl;

	std::string line{};
	std::getline(read_stream, line);
	std::getline(read_stream, line);

	write_stream << packet::serialize(packet::ethernet{2, 3, "data"}) << std::endl;
	write_stream << packet::serialize(packet::ethernet{3, 4, "data"}) << std::endl;
	write_stream << packet::serialize(packet::ethernet{4, 5, "data"}) << std::endl;

	auto is_open = read_stream.is_open();
	auto eofbit = read_stream.eof();
	auto badbit = read_stream.bad();
	auto failbit = read_stream.fail();

	read_stream.clear();

	std::getline(read_stream, line);
	std::getline(read_stream, line);
	std::getline(read_stream, line);
	std::getline(read_stream, line);
}

TEST_F(bridge_test, should_broadcast_packet_when_it_has_broadcast_id_as_destination_id) {
	packet::ethernet packet{99, 1, "data"};

	link::link link11{1, 1};
	link11.write_port(packet);

	bridge::bridge bridge{1, 3, {}};
	bridge.process_packets();

	// This value should be empty, since it is the port that the packet was sent over, and we do not broadcast packets
	// over the port from which they came.
	auto packet_from_link11 = link11.read_port();

	ASSERT_FALSE(packet_from_link11);

	// Since the packet did not come over this link it should have been broadcast to this link and we should be able
	// to read the packet on this link.
	link::link link12{1, 2};
	auto packet_from_link12 = link12.read_port();

	ASSERT_TRUE(packet_from_link12.has_value());
	EXPECT_EQ(packet, std::get<packet::ethernet>(packet_from_link12.value()));

	// Same idea here.
	link::link link13{1, 3};
	auto packet_from_link13 = link13.read_port();

	ASSERT_TRUE(packet_from_link13.has_value());
	EXPECT_EQ(packet, std::get<packet::ethernet>(packet_from_link13.value()));
}

TEST_F(bridge_test, should_broadcast_ethernet_packet_with_unknown_id) {
	packet::ethernet packet{2, 1, "data"};

	link::link link11{1, 1};
	link11.write_port(packet);

	bridge::bridge bridge{1, 3, {}};
	bridge.process_packets();

	auto packet_from_link11 = link11.read_port();

	ASSERT_TRUE(packet_from_link11.has_value());
	EXPECT_EQ(packet, std::get<packet::ethernet>(packet_from_link11.value()));

	link::link link12{1, 2};
	auto packet_from_link12 = link12.read_port();

	ASSERT_TRUE(packet_from_link12.has_value());
	EXPECT_EQ(packet, std::get<packet::ethernet>(packet_from_link12.value()));

	link::link link13{1, 3};
	auto packet_from_link13 = link13.read_port();

	ASSERT_TRUE(packet_from_link13.has_value());
	EXPECT_EQ(packet, std::get<packet::ethernet>(packet_from_link13.value()));
}

TEST_F(bridge_test, should_forward_ethernet_message_with_known_address_over_correct_port) {
	link::link link11{1, 1};

	link11.write_port(packet::ethernet{2, 1, "data"});

	bridge::bridge bridge{1, 3, {}};

	// After this point, the bridge sees that the ethernet id of 1 is behind port 1.
	bridge.process_packets();

	link11.read_port();

	link::link link12{1, 2};
	link12.read_port();

	link::link link13{1, 3};
	link13.read_port();

	// Now when the bridge sees a packet with a destination id of 1, it knows to send it over port 1.
	packet::ethernet packet{1, 2, ""};
	link13.write_port(packet);

	bridge.process_packets();

	// We should expect to see this packet only on the link behind port 1, and not the others, since the packet should
	// not have been broadcast, as we learned about the location of ethernet id 1 earlier.
	auto packet_from_link11 = link11.read_port();

	ASSERT_TRUE(packet_from_link11.has_value());
	EXPECT_EQ(packet, std::get<packet::ethernet>(packet_from_link11.value()));

	ASSERT_FALSE(link12.read_port().has_value());
	ASSERT_FALSE(link13.read_port().has_value());
}
