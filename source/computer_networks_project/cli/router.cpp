#include <thread>

#include <router/router.hpp>

using namespace computer_networks_project;

int main(int argc, char *argv[]) {
	std::vector<std::size_t> network_ids{};
	std::vector<std::size_t> host_ids{};
	std::vector<std::size_t> bridge_ids{};
	std::vector<std::size_t> port_ids{};
	std::vector<std::size_t> ethernet_ids{};

	for (std::size_t i = 1; i < argc; i += 5) {
		network_ids.push_back(std::stoull(argv[i]));
		host_ids.push_back(std::stoull(argv[i + 1]));
		bridge_ids.push_back(std::stoull(argv[i + 2]));
		port_ids.push_back(std::stoull(argv[i + 3]));
		ethernet_ids.push_back(std::stoull(argv[i + 4]));
	}
	router::router router{
		network_ids,
		host_ids,
		bridge_ids,
		port_ids,
		ethernet_ids
	};
	for (std::size_t i = 0;; ++i) {
		if (i % 5 == 0)
			router.hl_broadcast();

		router.process_packets();

		std::this_thread::sleep_for(std::chrono::seconds{1});
	}
}
