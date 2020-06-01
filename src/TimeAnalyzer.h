#ifndef _TIME_ANALYZER
#define _TIME_ANALYZER

#include <fstream>
#include <string>
#include <vector>
#include "Message.h"

class TimeAnalyzer {
public:
	static int outputTimeLog(const std::string& outputFile, const std::vector<Message>& messages, const std::vector<std::string>& participants);

private:
	struct DataPoint {
		int m_year = -1;
		int m_month = -1;
		int m_day = -1;
		std::vector<int> m_counts;
	};

	static int outputDataPoint(std::ofstream& file, const DataPoint& dataPoint);
};

#endif