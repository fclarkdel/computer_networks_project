#include <gtest/gtest.h>

#include <bridge/bridge.hpp>
#include <utils/utils.hpp>

using namespace computer_networks_project;

struct bridge_test : public ::testing::Test {
	~bridge_test() override {
		utils::remove_link_files_in_current_directory();
	}
};

TEST(bridge_test, should_learn_port_of_ethernet_address_through_source_address_of_ethernet_message) {
}

TEST(bridge_test, should_forward_ethernet_message_with_broadcast_address_over_all_ports) {
}

TEST(bridge_test, should_forward_ethernet_message_with_unknown_address_over_all_ports) {
}

TEST(bridge_test, should_forward_ethernet_message_with_known_address_over_correct_port) {
}
