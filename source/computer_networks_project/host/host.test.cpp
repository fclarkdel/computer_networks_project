#include <gtest/gtest.h>

#include <bridge/bridge.hpp>
#include <host/host.hpp>
#include <utils/utils.hpp>
#include <router/router.hpp>

using namespace computer_networks_project;

struct host_test : public ::testing::Test {
	~host_test() override {
		utils::remove_link_files_in_current_directory();
	}
};

TEST_F(host_test, should_broadcast_arp_req_for_da_packet_from_unknown_ip) {
	packet::ethernet packet{
		1,
		2,
		packet::serialize(
			packet::ip{
				1,
				2,
				2,
				1,
				packet::serialize(
					packet::da{
						1,
						1
					}
				)
			}
		)
	};
	link::link link11{1, 1};
	host::host host1{
		1,
		2,
		1,
		1,
		1,
		1,
		1
	};
	link11.write_bridge(packet);
	host1.process_packets();

	auto packet_read = link11.read_bridge();
}

TEST_F(host_test, should_send_arp_rep_for_an_arp_req_addressed_to_it) {
	packet::ethernet packet{
		1,
		2,
		packet::serialize(
			packet::arp{
				packet::arp_types::REQ,
				1,
				2,
				0,
				2,
				1,
				2
			}
		)
	};
	link::link link11{1, 1};
	host::host host1{
		1,
		2,
		1,
		1,
		1,
		1,
		1
	};
	link11.write_bridge(packet);
	host1.process_packets();

	auto packet_read = link11.read_bridge();
}

TEST_F(host_test, should_send_data_to_other_host) {
	host::host host1{
		1,
		2,
		1,
		1,
		1,
		1,
		1
	};
	host::host host2{
		1,
		3,
		2,
		1,
		1,
		1,
		2
	};
	bridge::bridge bridge1{1, 2, {}};
	host1.enqueue_data(1, 3, "1234512345123451223123421398675987234590823745098710298712908702139874098742134");

	for (std::size_t i = 0; i < 100; ++i) {
		host1.process_packets();
		bridge1.process_packets();
		host2.process_packets();
	}
}

TEST_F(host_test, should_send_data_to_host_on_other_network) {
	host::host host1{
		1,
		2,
		1,
		1,
		1,
		1,
		2
	};
	bridge::bridge bridge1{1, 2, {}};

	router::router router1{
		{1, 2},
		{1, 1},
		{1, 2},
		{1, 1},
		{2, 2}
	};
	bridge::bridge bridge2{2, 2, {}};

	host::host host2{
		2,
		2,
		3,
		2,
		1,
		2,
		2
	};
	host1.enqueue_data(2, 2, "1234512345123451223123421398675987234590823745098710298712908702139874098742134");

	for (std::size_t i = 0; i < 1000; ++i) {
		host1.process_packets();
		bridge1.process_packets();
		router1.process_packets();
		bridge2.process_packets();
		host2.process_packets();
	}
	host1.process_packets();
}