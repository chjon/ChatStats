#ifndef _WORD_ANALYZER
#define _WORD_ANALYZER

#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "Message.h"

class WordAnalyzer {
public:
	static int outputWordLog(const std::string& outputFile, bool isFirstFile, const std::vector<Message>& messages, const std::vector<std::string>& participants);
	static int analyze(const std::string& inFileStr, const std::string& outFileStr, const std::vector<std::string>& participants);

private:
	static std::string getNormalizedWord(std::stringstream& line);
	static int readWordFile(const std::string& inFileStr, unsigned int numParticipants, std::map<std::string, std::vector<int>>& wordCounts);
	static int writeWordFile(const std::string& outFileStr, const std::vector<std::string>& participants, std::map<std::string, std::vector<int>>& wordCounts);

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