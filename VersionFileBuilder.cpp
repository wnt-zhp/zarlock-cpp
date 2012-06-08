#include <cstdlib>
#include <fstream>
#include <iostream>
#include "version.h"

int main() {
#if defined(_WIN32)
	std::ofstream ofs("build_version_number_win32.txt");
#elif defined(__unix__)
	std::ofstream ofs("build_version_number_unix.txt");
#else
	std::ofstream ofs("build_version_number.txt");
#endif

	ofs << ZARLOK_VERSION;

	ofs.close();

	std::cout << ZARLOK_VERSION << std::endl;

	std::exit(EXIT_SUCCESS);
}
