#include "TimeAnalyzer.h"
#include <algorithm>
#include <iostream>

int TimeAnalyzer::outputTimeLog(const std::string& outputFile, bool isFirstFile, const std::vector<Message>& messages, const std::vector<std::string>& participants) {
	// Open output file
	std::ios_base::openmode flags = std::ios_base::out; // Default
	if (!isFirstFile) flags |= std::ios_base::app;
	std::ofstream outFile(outputFile, flags);
	if (!outFile.is_open()) return 1;
	
	// Output data points
	for (auto m = messages.rbegin(); m != messages.rend(); ++m) {
		const std::time_t messageTime = m->m_timestamp / 1000;
		outFile << messageTime << ',' << m->m_senderId << ',' << std::endl;
	}

	return 0;
}

int TimeAnalyzer::analyze(const std::string& inFileStr, const std::string& outFileStr, const std::vector<std::string>& participants) {
	std::vector<RawDataPoint> rawDataPoints;
	std::vector<DataPoint> dataPoints;
	TimeCompStrictness strictness = TimeCompStrictness::MONTH;

	// Read in data points
	if (readDataPoints(inFileStr, participants.size(), rawDataPoints)) return 1;

	// Sort data points
	std::sort(rawDataPoints.begin(), rawDataPoints.end(), [](const RawDataPoint& a, const RawDataPoint& b) {
		return a.m_time < b.m_time;
	});

	// Compress data points (by day, week, month, year)
	dataPoints = transformDataPoints(rawDataPoints, participants.size(), strictness);

	// Smooth out data points (sliding window average or EMA)
	// dataPoints = smoothEMA(dataPoints, participants.size(), 0.2);

	// Output data points
	writeDataPoints(outFileStr, participants, dataPoints, strictness);

	return 0;
}

int TimeAnalyzer::readDataPoints(const std::string& inFileStr, const unsigned int numParticipants, std::vector<RawDataPoint>& dataPoints) {
	std::ifstream inFile(inFileStr);
	if (inFile.bad()) return 1;

	const unsigned int maxLineLength = (sizeof(DataPoint) + numParticipants * sizeof(int));
	char* lineBuf = reinterpret_cast<char*>(malloc(maxLineLength * sizeof(char)));
	while (!inFile.eof() && !inFile.bad()) {
		inFile.getline(lineBuf, maxLineLength);
		if (!*lineBuf) continue;

		std::stringstream line(lineBuf);
		RawDataPoint cur;
		char curChar = 0;
		if (parseNum(line, &cur.m_time)) return 1;
		line >> curChar; if (curChar != ',') return 1;
		if (parseNum(line, &cur.m_senderId)) return 1;
		line >> curChar; if (curChar != ',') return 1;
		dataPoints.push_back(cur);
	}

	return 0;
}

std::vector<TimeAnalyzer::DataPoint> TimeAnalyzer::transformDataPoints(const std::vector<RawDataPoint>& original, const unsigned int numParticipants, TimeCompStrictness strictness) {
	std::vector<DataPoint> compressed;
	if (original.size() == 0) return compressed;

	// Compress and transform data points
	DataPoint first { original[0].m_time, std::vector<int>(numParticipants) };
	first.m_count[original[0].m_senderId] = 1;
	compressed.push_back(first);

	for (unsigned int i = 1; i < original.size(); ++i) {
		DataPoint& last = compressed[compressed.size() - 1];
		if (isSameTime(original[i].m_time, last.m_time, strictness)) {
			++last.m_count[original[i].m_senderId];
		} else {
			DataPoint tmp { original[i].m_time, std::vector<int>(numParticipants) };
			tmp.m_count[original[0].m_senderId] = 1;
			compressed.push_back(tmp);
		}
	}

	// Insert missing data points
	compressed = expandDataPoints(compressed, numParticipants, strictness);
	return compressed;
}

std::vector<TimeAnalyzer::DataPoint> TimeAnalyzer::expandDataPoints(const std::vector<DataPoint>& original, const unsigned int numParticipants, TimeCompStrictness strictness) {
	std::vector<DataPoint> expanded;
	if (original.size() == 0) return expanded;
	expanded.push_back(original[0]);
	for (unsigned int i = 1; i < original.size(); ++i) {
		std::time_t nextTime = getNextTime(expanded[expanded.size() - 1].m_time, strictness);
		while (!isSameTime(original[i].m_time, nextTime, strictness)) {
			expanded.push_back(DataPoint{nextTime, std::vector<int>(numParticipants)});
			nextTime = getNextTime(nextTime, strictness);
		}
		expanded.push_back(original[i]);
	}
	return expanded;
}

bool TimeAnalyzer::isSameTime(const std::time_t a, const std::time_t b, TimeCompStrictness strictness) {
	const std::tm aTimeInfo = *std::localtime(&a);
	const std::tm bTimeInfo = *std::localtime(&b);

	switch (strictness) {
		case TimeCompStrictness::DAY:
			return aTimeInfo.tm_year == bTimeInfo.tm_year && aTimeInfo.tm_yday == bTimeInfo.tm_yday;
		case TimeCompStrictness::MONTH:
			return aTimeInfo.tm_year == bTimeInfo.tm_year && aTimeInfo.tm_mon == bTimeInfo.tm_mon;
		case TimeCompStrictness::YEAR:
			return aTimeInfo.tm_year == bTimeInfo.tm_year;
		default: return a == b;
	}
}

std::time_t TimeAnalyzer::getNextTime(const std::time_t startTime, TimeCompStrictness strictness) {
	std::tm* startTimeInfo = std::localtime(&startTime);
	std::tm nextTimeInfo = {0};
	nextTimeInfo.tm_year = startTimeInfo->tm_year;
	nextTimeInfo.tm_mon = startTimeInfo->tm_mon;
	nextTimeInfo.tm_mday = startTimeInfo->tm_mday;
	switch (strictness) {
		case TimeCompStrictness::DAY:
			++nextTimeInfo.tm_mday; break;
		case TimeCompStrictness::MONTH:
			++nextTimeInfo.tm_mon; break;
		case TimeCompStrictness::YEAR:
			++nextTimeInfo.tm_year; break;
		default: return startTime + 1;
	}
	return std::mktime(&nextTimeInfo);
}

int TimeAnalyzer::writeDataPoints(const std::string& outFileStr, const std::vector<std::string>& participants, const std::vector<DataPoint>& dataPoints, TimeCompStrictness strictness) {
	std::ofstream outFile(outFileStr);
	if (outFile.bad()) return 1;

	// Write headers
	if (strictness <= TimeCompStrictness::YEAR) outFile << "year,";
	if (strictness <= TimeCompStrictness::MONTH) outFile << "month,";
	if (strictness <= TimeCompStrictness::DAY) outFile << "day,";
	for (const std::string& participant : participants) {
		outFile << participant << ',';
	}
	outFile << std::endl;

	// Write datapoints
	for (const DataPoint& dp : dataPoints) {
		const std::tm* timeInfo = std::localtime(&dp.m_time);
		if (strictness <= TimeCompStrictness::YEAR) outFile << timeInfo->tm_year + 1900 << ',';
		if (strictness <= TimeCompStrictness::MONTH) outFile << timeInfo->tm_mon + 1 << ',';
		if (strictness <= TimeCompStrictness::DAY) outFile << timeInfo->tm_mday << ',';

		for (unsigned int i = 0; i < dp.m_count.size(); ++i) {
			outFile << dp.m_count[i] << ',';
		}
		outFile << std::endl;
	}

	return 0;
}

std::vector<TimeAnalyzer::DataPoint> TimeAnalyzer::smoothEMA(const std::vector<DataPoint>& original, const unsigned int numParticipants, double alpha) {
	if (original.size() == 0) return original;
	std::vector<DataPoint> dataPoints(original.size());
	/*std::vector<double> prev(numParticipants);
	const double beta = 1.0 - alpha;

	// Initialize EMA calculation
	dataPoints[0] = original[0];
	for (unsigned int i = 0; i < dataPoints[0].m_count.size(); ++i) {
		prev[i] = dataPoints[0].m_counts[i];
	}

	for (unsigned int i = 1; i < original.size(); ++i) {
		dataPoints[i].m_year = original[i].m_year;
		dataPoints[i].m_month = original[i].m_month;
		dataPoints[i].m_day = original[i].m_day;
		for (unsigned int j = 0; j < original[i].m_counts.size(); ++j) {
			prev[j] = alpha * original[i].m_counts[j] + beta * prev[j];
			dataPoints[i].m_counts.push_back(prev[j]);
		}
	}*/

	return dataPoints;
}