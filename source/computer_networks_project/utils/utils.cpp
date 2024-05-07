#include <utils/utils.hpp>

namespace computer_networks_project::utils {
	void remove_link_files(std::string_view directory_absolute_path) {
		std::for_each(
			std::filesystem::directory_iterator{directory_absolute_path},
			std::filesystem::directory_iterator{},
			[](const std::filesystem::directory_entry &entry) {
				if (entry.is_regular_file() &&
					entry.path().filename().string().starts_with("toB") ||
					entry.path().filename().string().starts_with("fromB"))
					std::filesystem::remove(entry.path());
			}
		);
	}

	void remove_link_files_in_current_directory() {
		remove_link_files(
			std::filesystem::current_path().string()
		);
	}
}