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
	file >> curChar;
	while (curChar != DELIM_STR_END) {
		out += curChar;
		file >> curChar;
	}

	*str = std::move(out);
	return file.bad();
}

int JsonParser::parseNumber(std::ifstream& file, unsigned long long* num) {
	if (num == NULL) return 1;
	*num = 0;
	char curChar = file.peek();
	if (curChar < '0' || curChar > '9') return 1;
	while (curChar >= '0' && curChar <= '9') {
		file >> curChar;
		*num = (*num) * 10 + (curChar - '0');
		curChar = file.peek();
	}
	return 0;
}