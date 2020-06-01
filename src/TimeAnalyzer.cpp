#include "TimeAnalyzer.h"
#include <chrono>

int TimeAnalyzer::outputTimeLog(const std::string& outputFile, const std::vector<Message>& messages, const std::vector<std::string>& participants) {
	// Open output file
	std::ofstream outFile(outputFile, std::ios_base::app);
	if (!outFile.is_open()) return 1;
	
	// Output data points
	DataPoint cur;
	for (auto m = messages.rbegin(); m != messages.rend(); ++m) {
		std::time_t messageTime = m->m_timestamp / 1000;
		std::tm* timeInfo = std::localtime(&messageTime);

		if (timeInfo->tm_mday != cur.m_day || timeInfo->tm_mon != cur.m_month || timeInfo->tm_year != cur.m_year) {
			// Output the existing data point
			if (cur.m_year >= 0) {
				if (outputDataPoint(outFile, cur)) return 1;
			}

			// Reset current
			cur.m_year = timeInfo->tm_year;
			cur.m_month = timeInfo->tm_mon;
			cur.m_day = timeInfo->tm_mday;
			cur.m_counts = std::vector<int>(participants.size());
		}

		++cur.m_counts[m->m_senderId];
	}

	// Output the existing data point
	if (cur.m_year >= 0) {
		if (outputDataPoint(outFile, cur)) return 1;
	}

	return 0;
}

int TimeAnalyzer::outputDataPoint(std::ofstream& file, const DataPoint& dataPoint) {
	file
		<< dataPoint.m_year + 1900 << ','
		<< dataPoint.m_month + 1 << ','
		<< dataPoint.m_day << ',';

	for (int n : dataPoint.m_counts) {
		file << n << ',';
	}

	file << std::endl;
	return 0;
}