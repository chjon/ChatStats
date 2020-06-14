#ifndef _WORD_ANALYZER
#define _WORD_ANALYZER

#include <sstream>
#include <string>
#include <vector>
#include "Message.h"

class WordAnalyzer {
public:
	static int outputWordLog(const std::string& outputFile, bool isFirstFile, const std::vector<Message>& messages, const std::vector<std::string>& participants);

private:
	static std::string getNormalizedWord(std::stringstream& line);
};

#endif