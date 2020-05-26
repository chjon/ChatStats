#ifndef _JSON_PARSER
#define _JSON_PARSER

#include <fstream>
#include <functional>
#include <string>
#include <vector>

class JsonParser {
protected:
	static const char DELIM_OBJ_BGN = '{';
	static const char DELIM_OBJ_END = '}';
	static const char DELIM_ARR_BGN = '[';
	static const char DELIM_ARR_END = ']';
	static const char DELIM_NAM_SEP = ':';
	static const char DELIM_ITM_SEP = ',';
	static const char DELIM_STR_BGN = '"';
	static const char DELIM_STR_END = DELIM_STR_BGN;
	static const char DELIM_CTL_ESC = '\\';

	template<typename T>
	using ObjectFieldParser = std::function<int(std::ifstream&, T*)>;

	template<typename T>
	static int parseFile(const std::string& filename, T* obj, ObjectFieldParser<T> parseObjectFields) {
		std::ifstream file(filename);
		if (file.bad()) return 1;
		int err = parseObjectFields(file, obj);
		file.close();
		return file.bad() || err;
	}

	static int parseFieldName(std::ifstream& file, std::string* fieldName);

	static int parseString(std::ifstream& file, std::string* str);

	template<typename T>
	static int parseObject(std::ifstream& file, T* obj, ObjectFieldParser<T> parseObjectFields) {
		char curChar;
		file >> curChar; if (curChar != DELIM_STR_BGN) return 1;
		int err = parseObjectFields(file, obj);
		return err;
	}

	template<typename T>
	static int parseArray(std::ifstream& file, std::vector<T>* arr, ObjectFieldParser<T> parseObjectFields) {
		if (arr == NULL) return 1;

		char curChar;
		file >> curChar; if (curChar != DELIM_ARR_BGN) return 1;

		curChar = file.peek();
		while (curChar != DELIM_ARR_END) {
			T obj;
			if (parseObject(file, &obj, parseObjectFields)) return 1;
			arr->push_back(obj);
			file >> curChar; if (curChar != DELIM_ITM_SEP && curChar != DELIM_ARR_END) return 1;
		}

		return file.bad();
	}
};

#endif