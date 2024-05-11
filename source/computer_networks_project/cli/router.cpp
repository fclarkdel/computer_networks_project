#include <thread>

#include <router/router.hpp>

using namespace computer_networks_project;

int main(int argc, char *argv[]) {
	std::size_t id = std::stoull(argv[1]);
	std::size_t number_of_ports = std::stoull(argv[2]);
	std::vector<std::size_t> adjacent_bridge_ids{};

	for (std::size_t i = 3; i < argc; ++i)
		adjacent_bridge_ids.emplace_back(std::stoull(argv[i]));

	bridge::bridge bridge{id, number_of_ports, adjacent_bridge_ids};

	while (true) {
		bridge.process_packets();

		std::this_thread::sleep_for(std::chrono::seconds{1});
	}
}
