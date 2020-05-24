#include <iostream>
#include <string>

#include "Message.h"
#include "MessageParser.h"

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

	std::vector<Message> messages;
	MessageParser parser;
	if (!parser.parse(inputFile, messages)) {
		for (const Message& message : messages) {
			std::cout << message.content << std::endl;
		}
	}

	return 0;
}