#include "Message.h"

Message::Message(const unsigned int senderId, const unsigned long long timestamp, const std::string&& content) :
	m_senderId(senderId), m_timestamp(timestamp), m_content(content)
{}