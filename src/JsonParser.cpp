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
	if (file.peek() != DELIM_STR_BGN) return 1;
	char curChar;
	file >> curChar;
	
	std::string out;
	file >> curChar;
	while (curChar != DELIM_STR_END) {
		out += curChar;
		file >> curChar;
	}

	*str = std::move(out);
	return file.bad();
}