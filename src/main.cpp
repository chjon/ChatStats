#include <stdio.h>
#include <iostream>
#include <string>
#include <chrono>

#include "Message.h"
#include "MessageParser.h"

struct DataPoint {
	int m_year = -1;
	int m_month = -1;
	int m_day = -1;
	std::vector<int> m_counts;
};

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

	std::vector<std::string> participants;

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
			std::cerr << "Error while parsing " << inputFile << std::endl;
			return 1;
		}

		std::vector<DataPoint> dataPoints;
		DataPoint* last = NULL;
		for (auto m = messages.rbegin(); m != messages.rend(); ++m) {
			std::time_t messageTime = m->m_timestamp / 1000;
			std::tm* timeInfo = std::localtime(&messageTime);

			if (!last || timeInfo->tm_mday != last->m_day || timeInfo->tm_mon != last->m_month || timeInfo->tm_year != last->m_year) {
				std::vector<int> counts(participants.size());
				dataPoints.push_back(DataPoint{timeInfo->tm_year, timeInfo->tm_mon, timeInfo->tm_mday, counts});
				last = &dataPoints[dataPoints.size() - 1];
			}

			++last->m_counts[m->m_senderId];
		}

		for (const DataPoint d : dataPoints) {
			printf("%04d-%02d-%02d,", d.m_year + 1900, d.m_month + 1, d.m_day);
			for (unsigned int i = 0; i < d.m_counts.size(); ++i) {
				printf("%u,", d.m_counts[i]);
			}
			printf("\n");
		}
	}

	printf("date,");
	for (const std::string& participant : participants) {
		std::cout << participant << ",";
	}
	printf("\n");

	return 0;
}