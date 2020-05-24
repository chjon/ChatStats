#include "Message.h"

Message::Message(const unsigned int senderId, const unsigned long long timestamp, const std::string&& content) :
	senderId(senderId), timestamp(timestamp), content(content)
{}