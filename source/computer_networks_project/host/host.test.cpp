#include <gtest/gtest.h>

#include <bridge/bridge.hpp>
#include <host/host.hpp>
#include <utils/utils.hpp>

using namespace computer_networks_project;

struct host_test : public ::testing::Test {
	~host_test() override {
		utils::remove_link_files_in_current_directory();
	}
};

TEST_F(host_test, should_broadcast_arp_req_for_an_unknown_ip) {
	packet::ethernet packet{
		1,
		2,
		packet::serialize(
			packet::ip{
				1,
				2,
				2,
				1,
				"data"
			}
		)
	};
	link::link link11{1, 1};
	link::link link12{1, 2};
	link::link link13{1, 3};
	bridge::bridge bridge1{1, 3, {}};
	host::host host1{
		1,
		1,
		1,
		1,
		1
	};
	link11.write_port(packet);
	auto packet_read = link11.read_bridge();
	ASSERT_TRUE(packet_read.has_value());
	ASSERT_EQ(packet, std::get<packet::ethernet>(packet_read.value()));

	bridge1.process_packets();
	packet_read = link11.read_port();
	ASSERT_TRUE(packet_read.has_value());
	ASSERT_EQ(packet, std::get<packet::ethernet>(packet_read.value()));
	packet_read = link12.read_port();
	ASSERT_TRUE(packet_read.has_value());
	ASSERT_EQ(packet, std::get<packet::ethernet>(packet_read.value()));
	packet_read = link13.read_port();
	ASSERT_TRUE(packet_read.has_value());
	ASSERT_EQ(packet, std::get<packet::ethernet>(packet_read.value()));

	host1.process_packets();
	packet = packet::ethernet{
		99,
		1,
		packet::serialize(
			packet::arp{
				packet::arp_types::REQ,
				1,
				2,
				0,
				1,
				1,
				1
			}
		)
	};
	packet_read = link11.read_bridge();
	ASSERT_TRUE(packet_read.has_value());
	ASSERT_EQ(packet, std::get<packet::ethernet>(packet_read.value()));

	bridge1.process_packets();
	packet_read = link12.read_port();
	ASSERT_TRUE(packet_read.has_value());
	ASSERT_EQ(packet, std::get<packet::ethernet>(packet_read.value()));
	packet_read = link13.read_port();
	ASSERT_TRUE(packet_read.has_value());
	ASSERT_EQ(packet, std::get<packet::ethernet>(packet_read.value()));
}

TEST_F(host_test, should_send_arp_rep_for_an_arp_req_addressed_to_it) {
	packet::ethernet packet{
		1,
		3,
		packet::serialize(
			packet::ip{
				1,
				2,
				2,
				1,
				"data"
			}
		)
	};
	link::link link11{1, 1};
	link::link link12{1, 2};
	link::link link13{1, 3};
	bridge::bridge bridge1{1, 3, {}};
	host::host host1{
		1,
		1,
		1,
		1,
		1
	};
	host::host host2{
		1,
		2,
		1,
		3,
		2
	};
	link11.write_port(packet);
	auto packet_read = link11.read_bridge();
	ASSERT_TRUE(packet_read.has_value());
	ASSERT_EQ(packet, std::get<packet::ethernet>(packet_read.value()));

	bridge1.process_packets();
	packet_read = link11.read_port();
	ASSERT_TRUE(packet_read.has_value());
	ASSERT_EQ(packet, std::get<packet::ethernet>(packet_read.value()));
	packet_read = link12.read_port();
	ASSERT_TRUE(packet_read.has_value());
	ASSERT_EQ(packet, std::get<packet::ethernet>(packet_read.value()));
	packet_read = link13.read_port();
	ASSERT_TRUE(packet_read.has_value());
	ASSERT_EQ(packet, std::get<packet::ethernet>(packet_read.value()));

	host1.process_packets();
	packet = packet::ethernet{
		99,
		1,
		packet::serialize(
			packet::arp{
				packet::arp_types::REQ,
				1,
				2,
				0,
				1,
				1,
				1
			}
		)
	};
	packet_read = link11.read_bridge();
	ASSERT_TRUE(packet_read.has_value());
	ASSERT_EQ(packet, std::get<packet::ethernet>(packet_read.value()));

	bridge1.process_packets();
	packet_read = link12.read_port();
	ASSERT_TRUE(packet_read.has_value());
	ASSERT_EQ(packet, std::get<packet::ethernet>(packet_read.value()));
	packet_read = link13.read_port();
	ASSERT_TRUE(packet_read.has_value());
	ASSERT_EQ(packet, std::get<packet::ethernet>(packet_read.value()));

	host2.process_packets();
	packet = packet::ethernet{
		1,
		2,
		packet::serialize(
			packet::arp{
				packet::arp_types::REP,
				1,
				2,
				2,
				1,
				1,
				1
			}
		)
	};
	packet_read = link13.read_bridge();
	ASSERT_TRUE(packet_read.has_value());
	ASSERT_EQ(packet, std::get<packet::ethernet>(packet_read.value()));

	bridge1.process_packets();
	packet_read = link11.read_port();
	ASSERT_TRUE(packet_read.has_value());
	ASSERT_EQ(packet, std::get<packet::ethernet>(packet_read.value()));
	packet_read = link12.read_port();
	ASSERT_FALSE(packet_read);
	packet_read = link13.read_port();
	ASSERT_FALSE(packet_read);

	host1.process_packets();
	bridge1.process_packets();
	host2.process_packets();
}