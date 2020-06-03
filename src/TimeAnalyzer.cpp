#include "TimeAnalyzer.h"
#include <algorithm>
#include <chrono>
#include <iostream>

int TimeAnalyzer::outputTimeLog(const std::string& outputFile, bool isFirstFile, const std::vector<Message>& messages, const std::vector<std::string>& participants) {
	// Open output file
	std::ios_base::openmode flags = std::ios_base::out; // Default
	if (!isFirstFile) flags |= std::ios_base::app;
	std::ofstream outFile(outputFile, flags);
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

			// Reset current data point
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

int TimeAnalyzer::analyze(const std::string& inFileStr, const std::string& outFileStr, const std::vector<std::string>& participants) {
	std::vector<DataPoint> dataPoints;

	// Read in data points
	if (readDataPoints(inFileStr, participants.size(), dataPoints)) return 1;

	// Sort data points
	std::sort(dataPoints.begin(), dataPoints.end(), [](const DataPoint& a, const DataPoint& b){
		if (a.m_year < b.m_year) return true;
		if (a.m_year > b.m_year) return false;
		if (a.m_month < b.m_month) return true;
		if (a.m_month > b.m_month) return false;
		if (a.m_day < b.m_day) return true;
		return false;
	});

	// Compress data points (by day, week, month, year)
	dataPoints = compressDataPoints(dataPoints);

	// Output data points
	writeDataPoints(outFileStr, participants, dataPoints);

	return 0;
}

int TimeAnalyzer::readDataPoints(const std::string& inFileStr, const unsigned int numParticipants, std::vector<DataPoint>& dataPoints) {
	std::ifstream inFile(inFileStr);
	if (inFile.bad()) return 1;

	const unsigned int maxLineLength = (sizeof(DataPoint) + numParticipants * sizeof(int));
	char* lineBuf = reinterpret_cast<char*>(malloc(maxLineLength * sizeof(char)));
	while (!inFile.eof() && !inFile.bad()) {
		inFile.getline(lineBuf, maxLineLength);
		if (!*lineBuf) continue;

		std::stringstream line(lineBuf);
		DataPoint cur;
		cur.m_counts = std::vector<int>(numParticipants);
		char curChar = 0;
		if (parseNum(line, &cur.m_year)) return 1;
		line >> curChar; if (curChar != ',') return 1;
		if (parseNum(line, &cur.m_month)) return 1;
		line >> curChar; if (curChar != ',') return 1;
		if (parseNum(line, &cur.m_day)) return 1;
		line >> curChar; if (curChar != ',') return 1;

		unsigned int i = 0;
		do {
			if (parseNum(line, &cur.m_counts[i])) return 1;
			line >> curChar; if (curChar != ',') return 1;
			curChar = line.peek();
			++i;
		} while (curChar >= '0' && curChar <= '9');
		dataPoints.push_back(cur);
	}

	return 0;
}

std::vector<TimeAnalyzer::DataPoint> TimeAnalyzer::compressDataPoints(const std::vector<DataPoint>& original) {
	std::vector<DataPoint> compressed;
	if (original.size() == 0) return compressed;

	compressed.push_back(original[0]);
	DataPoint* last = &compressed[0];

	for (unsigned int i = 1; i < original.size(); ++i) {
		const DataPoint& cur = original[i];
		if (cur.sameDate(*last)) {
			for (unsigned int j = 0; j < cur.m_counts.size(); ++j) {
				last->m_counts[j] += cur.m_counts[j];
			}
		} else {
			compressed.push_back(original[i]);
			last = &compressed[compressed.size() - 1];
		}
	}

	return compressed;
}

int TimeAnalyzer::writeDataPoints(const std::string& outFileStr, const std::vector<std::string>& participants, const std::vector<DataPoint>& dataPoints) {
	std::ofstream outFile(outFileStr);
	if (outFile.bad()) return 1;

	// Write headers
	outFile << "date,";
	for (const std::string& participant : participants) {
		outFile << participant << ',';
	}
	outFile << std::endl;

	// Write datapoints
	for (const DataPoint& dp : dataPoints) {
		outFile << dp.m_year << '-' << dp.m_month << '-' << dp.m_day << ',';
		for (unsigned int i = 0; i < dp.m_counts.size(); ++i) {
			outFile << dp.m_counts[i] << ',';
		}
		outFile << std::endl;
	}

	return 0;
}