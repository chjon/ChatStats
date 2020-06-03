#ifndef _TIME_ANALYZER
#define _TIME_ANALYZER

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "Message.h"

class TimeAnalyzer {
public:
	static int outputTimeLog(const std::string& outputFile, bool isFirstFile, const std::vector<Message>& messages, const std::vector<std::string>& participants);
	static int analyze(const std::string& inFileStr, const std::string& outFileStr, const std::vector<std::string>& participants);

private:
	struct DataPoint {
		int m_year = -1;
		int m_month = -1;
		int m_day = -1;
		std::vector<int> m_counts;

		bool sameDate (const DataPoint& other) const {
			return
				m_year == other.m_year &&
				m_month == other.m_month &&
				m_day == other.m_day;
		}
	};

	static int outputDataPoint(std::ofstream& file, const DataPoint& dataPoint);
	static int readDataPoints(const std::string& inFileStr, const unsigned int numParticipants, std::vector<DataPoint>& dataPoints);
	static int writeDataPoints(const std::string& outFileStr, const std::vector<std::string>& participants, const std::vector<DataPoint>& dataPoints);
	static std::vector<DataPoint> compressDataPoints(const std::vector<DataPoint>& dataPoints);

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