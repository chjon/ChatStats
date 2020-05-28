#include "JsonParser.h"

int JsonParser::parseFieldName(std::ifstream& file, std::string* fieldName) {
	if (fieldName == NULL) return 1;
	if (parseString(file, fieldName)) return 1;

	char curChar;
	file >> curChar; if (curChar != DELIM_NAM_SEP) return 1;
	return file.bad();
}

int JsonParser::parseString(std::ifstream& file, std::string* str) {
	if (str == NULL) return 1;

	char curChar;
	file >> curChar; if (curChar != DELIM_STR_BGN) return 1;
	
	std::string out;
	file.get(curChar);
	while (curChar != DELIM_STR_END) {
		// Handle control sequences
		if (curChar == DELIM_CTL_ESC) {
			file.get(curChar);

			// Handle unicode
			if (curChar == 'u') {
				out += DELIM_CTL_ESC;
			}
		}
		out += curChar;
		file.get(curChar);
	}

	*str = std::move(out);
	return file.bad();
}

int JsonParser::parseNumber(std::ifstream& file, unsigned long long* num) {
	if (num == NULL) return 1;
	*num = 0;
	char curChar;
	file >> curChar;
	if (curChar < '0' || curChar > '9') return 1;
	while (curChar >= '0' && curChar <= '9') {
		*num = (*num) * 10 + (curChar - '0');
		file.get(curChar);
	}
	file.putback(curChar);
	return file.bad();
}

int JsonParser::parseBoolean(std::ifstream& file, bool* b) {
	if (b == NULL) return 1;
	char curChar;
	file >> curChar;
	if (curChar == 't') {
		file.get(curChar); if (curChar != 'r') return 1;
		file.get(curChar); if (curChar != 'u') return 1;
		file.get(curChar); if (curChar != 'e') return 1;
		*b = true;
	} else if (curChar == 'f') {
		file.get(curChar); if (curChar != 'a') return 1;
		file.get(curChar); if (curChar != 'l') return 1;
		file.get(curChar); if (curChar != 's') return 1;
		file.get(curChar); if (curChar != 'e') return 1;
		*b = false;
	} else {
		return 1;
	}
	
	return file.bad();
}