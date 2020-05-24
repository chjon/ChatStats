#include <iostream>
#include <string>

int main(const int argc, const char* const* argv) {
	// Validate options
	if (argc != 3) {
		std::cerr << "Usage: " <<
			argv[0] << " <INPUT_FILE> <OUTPUT_FILE>" <<
			std::endl;
		return 1;
	}

	std::string inputFile(argv[1]);
	std::string outputFile(argv[2]);

	return 0;
}