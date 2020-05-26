#include "MessageParser.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace std::placeholders;

int MessageParser::parse(const std::string& filename, std::vector<Message>& messages, std::vector<std::string>& participantNames) {
	std::vector<Participant> participants;
	MessageSection section { &participants, &messages };
	ObjectFieldParser<MessageSection> f = std::bind(&MessageParser::parseMessageSection, this, _1, _2);
	return parseFile(filename, &section, f);
}

int MessageParser::parseParticipant(std::ifstream& file, Participant* obj) {
	if (parseString(file, &obj->m_name)) return 1;
	std::cout << "Succeeded Participant Parsing" << std::endl;
	return 0;
}

int MessageParser::parseMessage(std::ifstream& file, Message* obj) {
	char curChar = file.peek();
	bool foundSender = false, foundTime = false, foundContent = false;
	std::string sender;
	std::string fieldName;

	while (curChar == DELIM_ITM_SEP) {
		file >> curChar;

		if (parseFieldName(file, &fieldName)) return 1;
		if (fieldName == "sender_name") {
			if (foundSender) return 1;
			if (parseString(file, &sender)) return 1;
			foundSender = true;
		} else if (fieldName == "timestamp_ms") {
		
		} else if (fieldName == "content") {

		} else if (fieldName == "reactions") {

		} else if (fieldName == "type") {
			
		} else {
			return 1;
		}

		curChar = file.peek();
	}

	std::cout << "Succeeded Message Parsing" << std::endl;
	return 0;
}

int MessageParser::parseMessageSection(std::ifstream& file, MessageSection* obj) {
	char curChar;
	bool foundParticipants = false, foundMessages = false;
	std::string fieldName;

	while (!foundParticipants || !foundMessages) {
		if (parseFieldName(file, &fieldName)) return 1;
		if (fieldName == "participants") {
			if (foundParticipants) return 1;
			ObjectFieldParser<Participant> f = std::bind(&MessageParser::parseParticipant, this, _1, _2);
			if (parseArray(file, obj->m_participants, f)) return 1;
			foundParticipants = true;
		} else if (fieldName == "messages") {
			if (foundMessages) return 1;
			ObjectFieldParser<Message> f = std::bind(&MessageParser::parseMessage, this, _1, _2);
			if (parseArray(file, obj->m_messages, f)) return 1;
			foundMessages = true;
		} else {
			return 1;
		}

		if (file.peek() != DELIM_ITM_SEP) break;
		file >> curChar;
	}

	std::cout << "Succeeded MessageSection Parsing" << std::endl;
	return foundParticipants && foundMessages;
}