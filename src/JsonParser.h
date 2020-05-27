#ifndef _JSON_PARSER
#define _JSON_PARSER

#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
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

	struct ParserMapValue {
	public:
		ParserMapValue(bool required) :
			m_required(required)
		{}

		virtual int parse(std::ifstream& file) = 0;

		bool m_required = true;
	};

	template<typename T>
	struct ParserMapValueObj : public ParserMapValue {
	private:
		T* m_obj = NULL;
		ObjectFieldParser<T>& m_parser;
	public:
		ParserMapValueObj(bool required, T* obj, ObjectFieldParser<T>& parser) :
			ParserMapValue(required), m_obj(obj), m_parser(parser)
		{}

		int parse(std::ifstream& file) {
			return m_parser(file, m_obj);
		}
	};

	template<typename T>
	struct ParserMapValueArr : public ParserMapValue {
	private:
		std::vector<T>* m_arr = NULL;
		ObjectFieldParser<T>& m_parser;
	public:
		ParserMapValueArr(bool required, std::vector<T>* arr, ObjectFieldParser<T>& parser) :
			ParserMapValue(required), m_arr(arr), m_parser(parser)
		{}

		int parse(std::ifstream& file) {
			return parseArray(file, m_arr, m_parser);
		}
	};

	struct ParserMapValueStr : public ParserMapValue {
	private:
		std::string* m_str = NULL;
	public:
		ParserMapValueStr(bool required, std::string* str) :
			ParserMapValue(required), m_str(str)
		{}

		int parse(std::ifstream& file) {
			return parseString(file, m_str);
		}
	};

	struct ParserMapValueNum : public ParserMapValue {
	private:
		unsigned long long* m_num = NULL;
	public:
		ParserMapValueNum(bool required, unsigned long long* num) :
			ParserMapValue(required), m_num(num)
		{}

		int parse(std::ifstream& file) {
			return parseNumber(file, m_num);
		}
	};

	template<typename T>
	static int parseFile(const std::string& filename, T* obj, ObjectFieldParser<T> objectFieldParser) {
		std::ifstream file(filename);
		if (file.bad()) return 1;
		int err = objectFieldParser(file, obj);
		file.close();
		return file.bad() || err;
	}

	static int parseFieldName(std::ifstream& file, std::string* fieldName);

	static int parseString(std::ifstream& file, std::string* str);

	static int parseNumber(std::ifstream& file, unsigned long long* num);

	template<typename T>
	static int parseObject(std::ifstream& file, T* obj, std::unordered_map<std::string, ParserMapValue*>& objectFieldParsers) {
		char curChar;
		file >> curChar; if (curChar != DELIM_OBJ_BGN) return 1;

		std::unordered_set<std::string> seen;
		std::string fieldName;

		// Parse fields
		curChar = DELIM_ITM_SEP;
		while (curChar == DELIM_ITM_SEP) {
			if (parseFieldName(file, &fieldName)) return 1;
			if (objectFieldParsers.find(fieldName) == objectFieldParsers.end()) { std::cout << "unexpected field " << fieldName << std::endl; return 1; }
			if (seen.find(fieldName) != seen.end()) { std::cout << "duplicate field " << fieldName << std::endl; return 1; }
			if (objectFieldParsers.find(fieldName)->second->parse(file)) { std::cout << "could not parse field " << fieldName << std::endl; return 1; }
			seen.emplace(fieldName);
			file >> curChar;
		}
		if (curChar != DELIM_OBJ_END) return 1;

		// Ensure that required fields have been found
		for (auto iter : objectFieldParsers) {
			if (iter.second->m_required && seen.find(iter.first) == seen.end()) return 1;
		}

		return file.bad();
	}

	template<typename T>
	static int parseArray(std::ifstream& file, std::vector<T>* arr, ObjectFieldParser<T> objectFieldParser) {
		if (arr == NULL) return 1;

		char curChar;
		file >> curChar; if (curChar != DELIM_ARR_BGN) return 1;

		curChar = file.peek();
		while (curChar != DELIM_ARR_END) {
			T obj;
			if (objectFieldParser(file, &obj)) return 1;
			arr->push_back(obj);
			file >> curChar; if (curChar != DELIM_ITM_SEP && curChar != DELIM_ARR_END) return 1;
		}

		return file.bad();
	}

	template<typename T>
	static int parseArray(std::ifstream& file, std::vector<T>* arr, const std::unordered_map<std::string, ParserMapValue*>& objectFieldParsers) {
		using namespace std::placeholders;
		ObjectFieldParser<T> f = std::bind(parseObject<T>, _1, _2, objectFieldParsers);
		return parseArray(file, arr, f);
	}
};

#endif