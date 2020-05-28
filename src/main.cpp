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

	std::vector<std::string> participants;
	std::vector<Message> messages;
	MessageParser parser;
	if (!parser.parse(inputFile, messages, participants)) {
		std::cout << "Parsed " << messages.size() << " messages" << std::endl;
		std::vector<int> messageCount(participants.size());
		for (unsigned int i = 0; i < participants.size(); ++i) {
			messageCount[i] = 0;
		}

		for (Message message : messages) {
			++messageCount[message.m_senderId];
		}

		for (unsigned int i = 0; i < participants.size(); ++i) {
			std::cout << participants[i] << ": " << messageCount[i] << std::endl;
		}
	} else {
		std::cerr << "Error while parsing" << std::endl;
		return 1;
	}

	return 0;
}