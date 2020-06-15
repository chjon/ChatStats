#include "WordAnalyzer.h"
#include <fstream>

int WordAnalyzer::outputWordLog(const std::string& outputFile, bool isFirstFile, const std::vector<Message>& messages, const std::vector<std::string>& participants) {
	// Open output file
	std::ios_base::openmode flags = std::ios_base::out; // Default
	if (!isFirstFile) flags |= std::ios_base::app;
	std::ofstream outFile(outputFile, flags);
	if (!outFile.is_open()) return 1;
	
	// Count words
	std::map<std::string, std::vector<int>> wordCount;
	for (auto m = messages.begin(); m != messages.end(); ++m) {
		std::stringstream wordStream(m->m_content);
		while (wordStream.good()) {
			const std::string word = getNormalizedWord(wordStream);
			if (word.size() == 0) continue;

			auto found = wordCount.find(word);
			if (found == wordCount.end()) {
				std::vector<int> count(participants.size());
				++count[m->m_senderId];
				wordCount.emplace(word, count);
			} else {
				++found->second[m->m_senderId];
			}
		}
	}

	// Output words
	for (auto iter : wordCount) {
		outFile << iter.first << ',';
		for (unsigned int i = 0; i < iter.second.size(); ++i) {
			outFile << iter.second[i] << ',';
		}
		outFile << std::endl;
	}

	return 0;
}

std::string WordAnalyzer::getNormalizedWord(std::stringstream& line) {
	std::string word;
	std::string normalized;
	line >> word;

	// Handle emoticons and emojis
	if (word[0] == ':' || word[0] == '\\') {
		for (unsigned int i = 0; i < word.size(); ++i) {
			if (word[i] == ',') normalized += '\'';
			else normalized += word[i];
		}
		return normalized;
	}

	// Handle normal text
	for (unsigned int i = 0; i < word.size(); ++i) {
		if (word[i] >= '0' && word[i] <= '9') {
			normalized += word[i];
		} else if (word[i] >= 'A' && word[i] <= 'Z') {
			normalized += word[i] - 'A' + 'a';
		} else if ((word[i] >= 'a' && word[i] <= 'z') || word[i] == '\'' || word[i] == '-') {
			normalized += word[i];
		} else {
			for (unsigned int j = word.size() - 1; j > i; --j) {
				line.putback(word[j]);
			}
			break;
		}
	}
	return normalized;
}

int WordAnalyzer::analyze(const std::string& inFileStr, const std::string& outFileStr, const std::vector<std::string>& participants) {
	std::map<std::string, std::vector<int>> wordCounts;
	if (readWordFile(inFileStr, participants.size(), wordCounts)) return 1;
	if (writeWordFile(outFileStr, participants, wordCounts)) return 1;
	return 0;
}

int WordAnalyzer::readWordFile(const std::string& inFileStr, unsigned int numParticipants, std::map<std::string, std::vector<int>>& wordCounts) {
	std::ifstream inFile(inFileStr);
	if (!inFile.is_open()) return 1;

	const unsigned int maxLineLength = 2048;
	char* lineBuf = reinterpret_cast<char*>(malloc(maxLineLength * sizeof(char)));
	while (!inFile.eof() && !inFile.bad()) {
		inFile.getline(lineBuf, maxLineLength);
		if (!*lineBuf) continue;

		std::stringstream line(lineBuf);
		char curChar;
		line >> curChar;
		std::string word;
		while (curChar != ',') {
			word += curChar;
			line >> curChar;
		}
		if (line.bad()) return 1;
		
		const auto found = wordCounts.find(word);
		std::vector<int> wordCount(numParticipants);
		if (found != wordCounts.end()) {
			wordCount = found->second;
		}

		for (unsigned int i = 0; i < numParticipants; ++i) {
			int count = 0;
			if (parseNum(line, &count)) return 1;
			line >> curChar; if (line.bad() || curChar != ',') return 1;
			wordCount[i] += count;
		}
		wordCounts.emplace(word, wordCount);
	}

	return 0;
}

int WordAnalyzer::writeWordFile(const std::string& outFileStr, const std::vector<std::string>& participants, std::map<std::string, std::vector<int>>& wordCounts) {
	std::ofstream outFile(outFileStr);
	if (!outFile.is_open()) return 1;

	// Output header
	outFile << "word,";
	for (std::string participant : participants) {
		outFile << participant << ",";
	}
	outFile << std::endl;

	// Output words
	for (auto iter : wordCounts) {
		outFile << iter.first << ',';
		for (unsigned int i = 0; i < iter.second.size(); ++i) {
			outFile << iter.second[i] << ',';
		}
		outFile << std::endl;
	}

	return 0;
}