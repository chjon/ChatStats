#ifndef _MESSAGE_PARSER
#define _MESSAGE_PARSER

#include <fstream>
#include <string>
#include <vector>
#include "JsonParser.h"
#include "Message.h"


class MessageParser : public JsonParser {
public:
	int parse(const std::string& filename, std::vector<Message>& messages, std::vector<std::string>& participants);

private:
	struct Participant {
		std::string m_name;
	};

	struct MessageSection {
		std::vector<Participant> m_participants;
		std::vector<Message> m_messages;
	};

	//template<typename T>
	// int parseObject(std::ifstream& file, T* obj);
	int parseMessage(std::ifstream& file, Message* obj);
	int parseMessageSection(std::ifstream& file, MessageSection* obj);
	int parseParticipant(std::ifstream& file, Participant* obj);
};

#endif