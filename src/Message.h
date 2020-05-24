#ifndef _MESSAGE
#define _MESSAGE

#include <string>

class Message {
public:
	Message(const unsigned int senderId, const unsigned long long timestamp, const std::string&& content);

	const unsigned int senderId;
	const unsigned long long timestamp;
	const std::string content;
};

#endif