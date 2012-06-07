#include <cstdlib>
#include <fstream>
#include <iostream>
#include "version.h"

int main() {
	std::ofstream ofs("build_version_number.txt");

	ofs << ZARLOK_VERSION;

	ofs.close();

	std::cout << ZARLOK_VERSION << std::endl;

	std::exit(EXIT_SUCCESS);
}
