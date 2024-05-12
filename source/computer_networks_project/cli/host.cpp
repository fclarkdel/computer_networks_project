#include <thread>

#include <host/host.hpp>

using namespace computer_networks_project;

int main(int argc, char *argv[]) {
	std::size_t network_id = std::stoull(argv[1]);
	std::size_t host_id = std::stoull(argv[2]);
	std::size_t ethernet_id = std::stoull(argv[3]);
	std::size_t router_network_id = std::stoull(argv[4]);
	std::size_t router_host_id = std::stoull(argv[5]);
	std::size_t bridge_id = std::stoull(argv[6]);
	std::size_t port_id = std::stoull(argv[7]);
	host::host host{
		network_id,
		host_id,
		ethernet_id,
		router_network_id,
		router_host_id,
		bridge_id,
		port_id
	};
	if (argc >= 10) {
		std::size_t destination_network_id = std::stoull(argv[8]);
		std::size_t destination_host_id = std::stoull(argv[9]);
		std::string data{argv[10]};

		for (std::size_t i = 11; i < argc; ++i)
			data.append(" ").append(argv[i]);

		host.enqueue_data(
			destination_network_id,
			destination_host_id,
			data
		);
	}
	while (true) {
		host.process_packets();

		std::this_thread::sleep_for(std::chrono::seconds{1});
	}
}
