#include "MessageParser.h"

#include <fstream>

int MessageParser::parse(const std::string& filename, std::vector<Message>& messages) {
	if (openFile(filename)) {
		return 1;
	}

	// Do actual parsing

	closeFile();
	return 0;
}

int MessageParser::openFile(const std::string& filename) {
	m_file = new std::ifstream(filename);
	if (!m_file) {
		return 1;
	}

	return 0;
}

int MessageParser::closeFile() {
	if (m_file) {
		delete m_file;
		m_file = NULL;
	}

	return 0;
}