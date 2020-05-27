#include "MessageParser.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace std::placeholders;

int MessageParser::parse(const std::string& filename, std::vector<Message>& messages, std::vector<std::string>& participantNames) {
	std::vector<Participant> participants;
	MessageSection section;
	ObjectFieldParser<MessageSection> f = std::bind(parseMessageSection, _1, &section);
	if (parseFile(filename, &section, f)) return 1;
	messages = section.m_messages;
	for (Participant p : section.m_participants) {
		participantNames.push_back(p.m_name);
	}
	return 0;
}

int MessageParser::parseParticipant(std::ifstream& file, Participant* obj) {
	std::unordered_map<std::string, ParserMapValue*> objectFieldParsers;

	ParserMapValueStr nameParserVal(true, &obj->m_name);
	objectFieldParsers.emplace("name", &nameParserVal);
	
	return parseObject(file, obj, objectFieldParsers);
}

int MessageParser::parseMessage(std::ifstream& file, Message* obj, std::vector<Participant>* participants) {
	std::unordered_map<std::string, ParserMapValue*> objectFieldParsers;
	
	std::string sender;
	ObjectFieldParser<unsigned int> senderParser = [&](std::ifstream& file, unsigned int* retval){
		if (participants == NULL) return 1;

		std::string sender;
		if (parseString(file, &sender)) return 1;
		for (unsigned int i = 0; i < participants->size(); ++i) {
			if (sender == (*participants)[i].m_name) {
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

	ParserMapValueStr contentParserVal(false, &obj->m_content);
	objectFieldParsers.emplace("content", &contentParserVal);

	std::string dummy;
	ParserMapValueStr dummyStrParserVal(false, &dummy);
	objectFieldParsers.emplace("type", &dummyStrParserVal);
	objectFieldParsers.emplace("ip", &dummyStrParserVal);
	
	std::vector<Reaction> reactions;
	ObjectFieldParser<Reaction> reactionParser = std::bind(parseReaction, _1, _2);
	ParserMapValueArr<Reaction> reactionParserVal(false, &reactions, reactionParser);
	objectFieldParsers.emplace("reactions", &reactionParserVal);

	Attachment attachment;
	std::vector<Attachment> attachments;
	ObjectFieldParser<Attachment> attachmentParser = std::bind(parseAttachment, _1, _2);
	ParserMapValueObj<Attachment> attachmentParserValObj(false, &attachment, attachmentParser);
	ParserMapValueArr<Attachment> attachmentParserValArr(false, &attachments, attachmentParser);
	objectFieldParsers.emplace("sticker", &attachmentParserValObj);
	objectFieldParsers.emplace("photos", &attachmentParserValArr);
	objectFieldParsers.emplace("videos", &attachmentParserValArr);
	objectFieldParsers.emplace("files", &attachmentParserValArr);
	objectFieldParsers.emplace("gifs", &attachmentParserValArr);

	Share share{};
	ObjectFieldParser<Share> shareParser = std::bind(parseShare, _1, _2);
	ParserMapValueObj<Share> shareParserVal(false, &share, shareParser);
	objectFieldParsers.emplace("share", &shareParserVal);

	return parseObject(file, obj, objectFieldParsers);
}

int MessageParser::parseMessageSection(std::ifstream& file, MessageSection* obj) {
	std::unordered_map<std::string, ParserMapValue*> objectFieldParsers;

	ObjectFieldParser<Participant> participantParser = std::bind(parseParticipant, _1, _2);
	ParserMapValueArr<Participant> participantParserVal(true, &obj->m_participants, participantParser);
	objectFieldParsers.emplace("participants", &participantParserVal);

	ObjectFieldParser<Message> messageParser = std::bind(parseMessage, _1, _2, &obj->m_participants);
	ParserMapValueArr<Message> messageParserVal(true, &obj->m_messages, messageParser);
	objectFieldParsers.emplace("messages", &messageParserVal);

	std::string title;
	ParserMapValueStr titleParserVal(true, &title);
	objectFieldParsers.emplace("title", &titleParserVal);

	bool isParticipant;
	ParserMapValueBool isParticipantParserVal(true, &isParticipant);
	objectFieldParsers.emplace("is_still_participant", &isParticipantParserVal);

	std::string dummy;
	ParserMapValueStr dummyParserVal(true, &dummy);
	objectFieldParsers.emplace("thread_type", &dummyParserVal);
	objectFieldParsers.emplace("thread_path", &dummyParserVal);
	
	return parseObject(file, obj, objectFieldParsers);
}

int MessageParser::parseReaction(std::ifstream& file, Reaction* obj) {
	std::unordered_map<std::string, ParserMapValue*> objectFieldParsers;

	ParserMapValueStr emojiParserVal(true, &obj->m_emoji);
	objectFieldParsers.emplace("reaction", &emojiParserVal);

	ParserMapValueStr actorParserVal(true, &obj->m_actor);
	objectFieldParsers.emplace("actor", &actorParserVal);
	
	return parseObject(file, obj, objectFieldParsers);
}

int MessageParser::parseAttachment(std::ifstream& file, Attachment* obj) {
	std::unordered_map<std::string, ParserMapValue*> objectFieldParsers;

	ParserMapValueStr uriParserVal(true, &obj->m_uri);
	objectFieldParsers.emplace("uri", &uriParserVal);

	ParserMapValueNum timestampParserVal(false, &obj->m_timestamp);
	objectFieldParsers.emplace("creation_timestamp", &timestampParserVal);

	Attachment thumbnail;
	ObjectFieldParser<Attachment> thumbnailParser = std::bind(parseAttachment, _1, _2);
	ParserMapValueObj<Attachment> thumbnailParserVal(false, &thumbnail, thumbnailParser);
	objectFieldParsers.emplace("thumbnail", &thumbnailParserVal);

	return parseObject(file, obj, objectFieldParsers);
}

int MessageParser::parseShare(std::ifstream& file, Share* obj) {
	std::unordered_map<std::string, ParserMapValue*> objectFieldParsers;

	ParserMapValueStr linkParserVal(true, &obj->m_link);
	objectFieldParsers.emplace("link", &linkParserVal);
	
	return parseObject(file, obj, objectFieldParsers);
}