#include "MessageParser.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace std::placeholders;

int MessageParser::parse(const std::string& filename, std::vector<Message>& messages, std::vector<std::string>& participantNames) {
	std::vector<Participant> participants;
	MessageSection section;
	ObjectFieldParser<MessageSection> f = std::bind(&MessageParser::parseMessageSection, this, _1, &section);
	if (parseFile(filename, &section, f)) return 1;
	messages = section.m_messages;
	for (Participant p : section.m_participants) {
		participantNames.push_back(p.m_name);
	}
	return 0;
}

int MessageParser::parseParticipant(std::ifstream& file, Participant* obj) {
	std::cout << "Parsing participant" << std::endl;
	return (parseString(file, &obj->m_name));
}

int MessageParser::parseMessage(std::ifstream& file, Message* obj) {
	std::cout << "Parsing message" << std::endl;
	std::string sender;
	std::unordered_map<std::string, ParserMapValue*> objectFieldParsers;

	ObjectFieldParser<unsigned int> senderParser = [this](std::ifstream& file, unsigned int* retval){
		if (m_participants == NULL) return 1;

		std::string sender;
		if (parseString(file, &sender)) return 1;
		for (unsigned int i = 0; i < m_participants->size(); ++i) {
			if (sender == (*m_participants)[i].m_name) {
				*retval = i;
				return 0;
			}
		}
		return 1;
	};

	ParserMapValueObj<unsigned int> senderParserVal(true, &obj->m_senderId, senderParser);
	objectFieldParsers.emplace("sender_name", &senderParserVal);

	ParserMapValueNum timeParserVal(true, &obj->m_timestamp);
	objectFieldParsers.emplace("timestamp_ms", &timeParserVal);
	
	return parseObject(file, obj, objectFieldParsers);
}

int MessageParser::parseMessageSection(std::ifstream& file, MessageSection* obj) {
	std::cout << "Parsing message section" << std::endl;
	std::unordered_map<std::string, ParserMapValue*> objectFieldParsers;

	ObjectFieldParser<Participant> participantParser = std::bind(&MessageParser::parseParticipant, this, _1, _2);
	ParserMapValueArr<Participant> participantParserVal(true, &obj->m_participants, participantParser);
	objectFieldParsers.emplace("participants", &participantParserVal);

	ObjectFieldParser<Message> messageParser = std::bind(&MessageParser::parseMessage, this, _1, _2);
	ParserMapValueArr<Message> messageParserVal(true, &obj->m_messages, messageParser);
	objectFieldParsers.emplace("messages", &messageParserVal);
	
	return parseObject(file, obj, objectFieldParsers);
}