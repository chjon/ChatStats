#ifndef _MESSAGE_PARSER
#define _MESSAGE_PARSER

#include <fstream>
#include <string>
#include <vector>
#include "JsonParser.h"
#include "Message.h"

class MessageParser : public JsonParser {
public:
	static int parse(const std::string& filename, std::vector<Message>& messages, std::vector<std::string>& participants);

private:
	struct Participant {
		std::string m_name;
	};

	struct MessageSection {
		std::vector<Participant> m_participants;
		std::vector<Message> m_messages;
	};

	struct Reaction {
		std::string m_emoji;
		std::string m_actor;
	};

	struct Attachment {
		std::string m_uri;
		unsigned long long m_timestamp = 0;
	};

	struct Share {
		std::string m_val;
	};

	//template<typename T>
	// int parseObject(std::ifstream& file, T* obj);
	static int parseMessage(std::ifstream& file, Message* obj, std::vector<Participant>* participants);
	static int parseMessageSection(std::ifstream& file, MessageSection* obj);
	static int parseParticipant(std::ifstream& file, Participant* obj);
	static int parseReaction(std::ifstream& file, Reaction* obj);
	static int parseAttachment(std::ifstream& file, Attachment* obj);
	static int parseShare(std::ifstream& file, Share* obj);
};

#endif