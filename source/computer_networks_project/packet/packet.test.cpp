#include <gtest/gtest.h>

#include <packet/packet.hpp>

using namespace computer_networks_project;

TEST(packet_test, should_return_serialization_if_serializing_valid_data) {
	EXPECT_EQ(packet::serialize(packet::ethernet{1, 0, "data"}), "ETHERNET 1 0 data");
	EXPECT_EQ(packet::serialize(packet::ip{1, 1, 2, 2, "data"}), "IP 1 1 2 2 data");
	EXPECT_EQ(packet::serialize(packet::arp{packet::arp_types::REQ, 1, 2, 0, 3, 2, 1}), "ARP REQ 1 2 3 2 1");
	EXPECT_EQ(packet::serialize(packet::hl{1, 1, 2}), "HL 1 1 2");
	EXPECT_EQ(packet::serialize(packet::bc{1, 1, 1, 2, "data"}), "BC 1 1 1 2 data");
}

TEST(packet_test, should_return_packet_if_deserializing_valid_data) {
	std::string serialized_packet = "ETHERNET 1 0 data";
	std::optional<packet::packet_types> deserialized_packet = packet::deserialize(serialized_packet);
	ASSERT_TRUE(deserialized_packet.has_value());
	EXPECT_EQ(get<packet::ethernet>(deserialized_packet.value()), (packet::ethernet{1, 0, "data"}));

	serialized_packet = "IP 1 1 2 2 data";
	deserialized_packet = packet::deserialize(serialized_packet);
	ASSERT_TRUE(deserialized_packet.has_value());
	EXPECT_EQ(get<packet::ip>(deserialized_packet.value()), (packet::ip{1, 1, 2, 2, "data"}));

	serialized_packet = "IP 1 1 2 2 ETHERNET 1 0 data";
	deserialized_packet = packet::deserialize(serialized_packet);
	ASSERT_TRUE(deserialized_packet.has_value());

	auto packet = get<packet::ip>(deserialized_packet.value());
	EXPECT_EQ(packet, (packet::ip{1, 1, 2, 2, "ETHERNET 1 0 data"}));

	serialized_packet = packet.data;
	deserialized_packet = packet::deserialize(serialized_packet);
	ASSERT_TRUE(deserialized_packet.has_value());
	EXPECT_EQ(get<packet::ethernet>(deserialized_packet.value()), (packet::ethernet{1, 0, "data"}));

	serialized_packet = "ARP REQ 1 2 3 2 1";
	deserialized_packet = packet::deserialize(serialized_packet);
	ASSERT_TRUE(deserialized_packet.has_value());
	EXPECT_EQ(get<packet::arp>(deserialized_packet.value()), (packet::arp{packet::arp_types::REQ, 1, 2, 0, 3, 2, 1}));

	serialized_packet = "HL 1 1 2";
	deserialized_packet = packet::deserialize(serialized_packet);
	ASSERT_TRUE(deserialized_packet.has_value());
	EXPECT_EQ(get<packet::hl>(deserialized_packet.value()), (packet::hl{1, 1, 2}));

	serialized_packet = "BC 1 1 1 2 data";
	deserialized_packet = packet::deserialize(serialized_packet);
	ASSERT_TRUE(deserialized_packet.has_value());
	EXPECT_EQ(get<packet::bc>(deserialized_packet.value()), (packet::bc{1, 1, 1, 2, "data"}));
}

TEST(packet_test, should_return_nothing_if_deserializing_invalid_data) {
	EXPECT_EQ(packet::deserialize("invalid_packet_type 1 0 data"), std::nullopt);
}
