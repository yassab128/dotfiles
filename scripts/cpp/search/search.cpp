#include <filesystem>
#include <iostream>
#include <set>

int
main(const int argc, const char *const *const argv)
{
	if (argc != 2) {
		return 1;
	}

	const std::string path_name = "..";

	std::set<std::string> sorted_by_name;

	for (const std::filesystem::directory_entry &entry :
	 std::filesystem::recursive_directory_iterator(path_name)) {
		if (entry.path().filename().string().find(argv[1]) ==
		    std::string::npos) {
			continue;
		} else if (entry.is_regular_file()) {
			sorted_by_name.insert(
			 "\e[1;32m" + entry.path().string() + "/");
		} else if (entry.is_directory()) {
			sorted_by_name.insert(
			 "\e[0;31m" + entry.path().string());
		}
	}

	std::cout << "\n";

	for (const std::string &filename : sorted_by_name) {
		std::cout << filename.c_str() << "\n";
	}

	std::cout << "\e[0m\n";
}
