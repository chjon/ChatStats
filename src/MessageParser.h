#ifndef _MESSAGE_PARSER
#define _MESSAGE_PARSER

#include <fstream>
#include <string>
#include <vector>
#include "Message.h"

class MessageParser {
public:
	int parse(const std::string& filename, std::vector<Message>& messages);

private:
	int openFile(const std::string& filename);
	int closeFile();

	std::ifstream* m_file = NULL;
};

#endif