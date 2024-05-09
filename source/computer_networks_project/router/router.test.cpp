#include <gtest/gtest.h>

#include <bridge/bridge.hpp>
#include <router/router.hpp>
#include <utils/utils.hpp>

using namespace computer_networks_project;

struct router_test : public ::testing::Test {
	~router_test() override {
		utils::remove_link_files_in_current_directory();
	}
};

TEST_F(router_test, should_broadcast_arp_req_for_an_unknown_ip) {
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
	router::router router1{
		{1},
		{1},
		{1},
		{1},
		{1}
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

	router1.process_packets();
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

TEST_F(router_test, should_send_arp_rep_for_an_arp_req_addressed_to_it) {
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
	router::router router1{
		{1},
		{1},
		{1},
		{1},
		{1}
	};
	router::router router2{
		{1},
		{2},
		{1},
		{3},
		{2}
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

	router1.process_packets();
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

	router2.process_packets();
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

	router1.process_packets();
	bridge1.process_packets();
	router2.process_packets();
}

TEST_F(router_test, should_broadcast_hl_packets_on_hello_broadcast) {
	link::link link11{1, 1};
	link::link link12{1, 2};
	link::link link21{2, 1};
	link::link link31{3, 1};
	bridge::bridge bridge1{1, 2, {}};
	router::router router1{
		{1, 2, 3},
		{5, 4, 3},
		{1, 2, 3},
		{1, 1, 1},
		{1, 1, 1}
	};
	router::router router2{
		{1},
		{1},
		{1},
		{2},
		{2}
	};
	router1.hello_broadcast();
	router2.hello_broadcast();
	auto packet = packet::deserialize(std::get<packet::ethernet>(link11.read_bridge().value()).data);
	ASSERT_TRUE(packet.has_value());
	ASSERT_EQ(
		(packet::hl{1, 5, 1}),
		std::get<packet::hl>(packet.value())
	);
	packet = packet::deserialize(std::get<packet::ethernet>(link21.read_bridge().value()).data);
	ASSERT_TRUE(packet.has_value());
	ASSERT_EQ(
		(packet::hl{2, 4, 1}),
		std::get<packet::hl>(packet.value())
	);
	packet = packet::deserialize(std::get<packet::ethernet>(link31.read_bridge().value()).data);
	ASSERT_TRUE(packet.has_value());
	ASSERT_EQ(
		(packet::hl{3, 3, 1}),
		std::get<packet::hl>(packet.value())
	);
	bridge1.process_packets();
	router1.process_packets();
}
