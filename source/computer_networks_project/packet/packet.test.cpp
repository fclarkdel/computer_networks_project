#include <gtest/gtest.h>

#include <packet/packet.hpp>

using namespace computer_networks_project;

TEST(packet_test, should_return_serialization_if_serializing_valid_data) {
	EXPECT_EQ(packet::serialize(packet::ethernet{1, 0, "data"}), "ethernet 1 0 data");
}

TEST(packet_test, should_return_packet_if_deserializing_valid_data) {
	std::string serialized_packet = "ethernet 1 0 data";
	packet::ethernet deserialized_packet = get<packet::ethernet>(packet::deserialize(serialized_packet).value());

	EXPECT_EQ(deserialized_packet, (packet::ethernet{1, 0, "data"}));
}

TEST(packet_test, should_return_nothing_if_deserializing_invalid_data) {
	EXPECT_EQ(packet::deserialize("invalid_packet_type 1 0 data"), std::nullopt);
}
