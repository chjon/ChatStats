#ifndef _MESSAGE
#define _MESSAGE

#include <string>

class Message {
public:
	Message(const unsigned int senderId = 0, const unsigned long long timestamp = 0, const std::string&& content = "");

	unsigned int m_senderId;
	long m_timestamp;
	std::string m_content;
};

#endif