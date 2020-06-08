#ifndef _TIME_ANALYZER
#define _TIME_ANALYZER

#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "Message.h"

class TimeAnalyzer {
public:
	enum class Granularity {
		DAY, MONTH, YEAR
	};

	static int outputTimeLog(const std::string& outputFile, bool isFirstFile, const std::vector<Message>& messages, const std::vector<std::string>& participants);
	static int analyze(const std::string& inFileStr, const std::string& outFileStr, const std::vector<std::string>& participants, Granularity granularity);

private:

	struct RawDataPoint {
		std::time_t m_time;
		int m_senderId;
	};

	struct DataPoint {
		std::time_t m_time;
		std::vector<int> m_count;
	};

	static int readDataPoints(const std::string& inFileStr, const unsigned int numParticipants, std::vector<RawDataPoint>& dataPoints);
	static int writeDataPoints(const std::string& outFileStr, const std::vector<std::string>& participants, const std::vector<DataPoint>& dataPoints, Granularity granularity);
	static bool isSameTime(const std::time_t a, const std::time_t b, Granularity granularity);
	static std::time_t getNextTime(const std::time_t time, Granularity granularity);

	/**
	 * @brief Convert raw data points into compressed data points
	 * @param dataPoints raw data points
	 * @param numParticipants number of participants
	 * @return compressed data points
	 */
	static std::vector<DataPoint> transformDataPoints(const std::vector<RawDataPoint>& dataPoints, const unsigned int numParticipants, Granularity granularity);
	static std::vector<DataPoint> expandDataPoints(const std::vector<DataPoint>& dataPoints, const unsigned int numParticipants, Granularity granularity);
	static std::vector<DataPoint> smoothEMA(const std::vector<DataPoint>& dataPoints, const unsigned int numParticipants, double alpha);

	template<typename T>
	static int parseNum(std::stringstream& s, T* num) {
		if (num == NULL) return 1;
		*num = 0;
		char curChar;
		s >> curChar;
		if (curChar < '0' || curChar > '9') return 1;
		while (curChar >= '0' && curChar <= '9') {
			*num = (*num) * 10 + (curChar - '0');
			s.get(curChar);
		}
		s.putback(curChar);
		return s.bad();
	}
};

#endif