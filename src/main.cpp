#include <iostream>
#include <string>
#include <chrono>

#include "Message.h"
#include "MessageParser.h"
#include "TimeAnalyzer.h"

int main(const int argc, const char* const* argv) {
	// Validate options
	if (argc < 3) {
		std::cerr << "Usage: " <<
			argv[0] << " <OUTPUT_FILE> <INPUT_FILE>..." <<
			std::endl;
		return 1;
	}

	int argIndex = 1;
	std::string outputFile(argv[argIndex++]);
	std::string timeLogFile = "timelog_temp.csv";

	std::vector<std::string> participants;

	// Parse input files
	bool isFirstFile = true;
	while (argIndex < argc) {
		std::string inputFile(argv[argIndex++]);
		std::vector<Message> messages;
		MessageParser parser;
		if (!parser.parse(inputFile, messages, participants)) {
			std::vector<int> messageCount(participants.size());
			for (unsigned int i = 0; i < participants.size(); ++i) {
				messageCount[i] = 0;
			}

			for (Message message : messages) {
				++messageCount[message.m_senderId];
			}
		} else {
			std::cerr << "Error while parsing: " << inputFile << std::endl;
			return 1;
		}

		// Add messages to time analysis
		if (TimeAnalyzer::outputTimeLog(timeLogFile, isFirstFile, messages, participants)) {
			std::cerr << "Error while generating time log: " << timeLogFile << std::endl;
			return 1;
		}

		isFirstFile = false;
	}

	// Do time analysis
	if (TimeAnalyzer::analyze(timeLogFile, outputFile, participants)) {
		std::cerr << "Error while analyzing time log: " << timeLogFile << std::endl;
		return 1;
	}

	// Do word analysis

	return 0;
}