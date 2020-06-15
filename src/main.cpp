#include <iostream>
#include <string>
#include <chrono>

#include "Message.h"
#include "MessageParser.h"
#include "TimeAnalyzer.h"
#include "WordAnalyzer.h"

int main(const int argc, const char* const* argv) {
	// Validate options
	if (argc < 4) {
		std::cerr << "Usage: " <<
			argv[0] << " <GRANULARITY> <OUTPUT_FILE> <INPUT_FILE>..." <<
			std::endl;
		return 1;
	}

	int argIndex = 1;

	// Validate granularity
	std::string granularityStr(argv[argIndex++]);
	if (granularityStr.size() != 1) {
		std::cerr << "Granularity must be one of: [d|m|y]" << std::endl;
		return 1;
	}

	TimeAnalyzer::Granularity granularity;
	switch (granularityStr[0]) {
		case 'd': granularity = TimeAnalyzer::Granularity::DAY; break;
		case 'm': granularity = TimeAnalyzer::Granularity::MONTH; break;
		case 'y': granularity = TimeAnalyzer::Granularity::YEAR; break;
		default:
			std::cerr << "Granularity must be one of: [d|m|y]" << std::endl;
			return 1;
	}

	std::string outputFile(argv[argIndex++]);
	std::string timeLogFile = "timelog_temp.csv";
	std::string wordLogFile = "wordlog_temp.csv";

	std::vector<std::string> participants;
	std::vector<unsigned int> numMessages;
	std::vector<unsigned long> messageLengths;

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

		// Add messages to word analysis
		if (WordAnalyzer::outputWordLog(wordLogFile, isFirstFile, messages, participants)) {
			std::cerr << "Error while generating word log: " << wordLogFile << std::endl;
			return 1;
		}

		// Add messages to total count
		for (Message m : messages) {
			while (m.m_senderId >= numMessages.size()) {
				numMessages.push_back(0);
				messageLengths.push_back(0);
			}

			numMessages[m.m_senderId] += 1;
			messageLengths[m.m_senderId] += m.m_content.size();
		}

		isFirstFile = false;
	}

	// Do time analysis
	if (TimeAnalyzer::analyze(timeLogFile, outputFile + "_time.csv", participants, granularity)) {
		std::cerr << "Error while analyzing time log: " << timeLogFile << std::endl;
		return 1;
	}

	// Do word analysis
	if (WordAnalyzer::analyze(wordLogFile, outputFile + "_word.csv", participants)) {
		std::cerr << "Error while analyzing word log: " << wordLogFile << std::endl;
		return 1;
	}

	// Output total word count and and message length
	std::cout << "name, count, total length" << std::endl;
	for (unsigned int i = 0; i < participants.size(); ++i) {
		std::cout << participants[i] << ", " << numMessages[i] << ", " << messageLengths[i] << std::endl;
	}

	return 0;
}