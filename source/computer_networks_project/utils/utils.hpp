#ifndef COMPUTER_NETWORKS_PROJECT_UTILS_HPP
#define COMPUTER_NETWORKS_PROJECT_UTILS_HPP

#include <filesystem>

namespace computer_networks_project::utils {
	void remove_link_files(std::string_view directory_absolute_path);

	void remove_link_files_in_current_directory();
}
#endif
