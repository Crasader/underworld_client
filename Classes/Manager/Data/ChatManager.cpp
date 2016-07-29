//
//  ChatManager.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/1.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ChatManager.h"
#include "cocostudio/CocoStudio.h"
#include "ChatData.h"
#include "ChatMark.h"

using namespace std;
using namespace cocostudio;

static ChatManager* s_pInstance(nullptr);
ChatManager* ChatManager::getInstance()
{
    if (!s_pInstance) {
        s_pInstance = new (nothrow) ChatManager();
    }
    
    return s_pInstance;
}

void ChatManager::purge()
{
    if (s_pInstance) {
        CC_SAFE_DELETE(s_pInstance);
    }
}

ChatManager::ChatManager()
{
    _mark = new (nothrow) ChatMark();
}

ChatManager::~ChatManager()
{
    clear();
    CC_SAFE_DELETE(_mark);
}

void ChatManager::clear()
{
    for(auto iter = _chatData.begin(); iter != _chatData.end(); ++iter){
        auto& vec(iter->second);
        for (auto data : vec) {
            CC_SAFE_DELETE(data);
        }
        
        vec.clear();
    }
    
    _chatData.clear();
}

void ChatManager::parseChannel(ChatType type, const rapidjson::Value& messages, const char* key)
{
    if (DICTOOL->checkObjectExist_json(messages, key)) {
        vector<ChatData*> vec;
        const rapidjson::Value& world = DICTOOL->getSubDictionary_json(messages, key);
        const int cnt = DICTOOL->getArrayCount_json(messages, key);
        for (int i = 0; i < cnt; ++i)
        {
            const rapidjson::Value& message = DICTOOL->getSubDictionary_json(world, i);
            ChatData* chatData = new ChatData(type, message);
            vec.push_back(chatData);
            
        }
        _chatData.insert(make_pair(type, vec));
    }
}

void ChatManager::parse(const rapidjson::Value& jsonDict)
{
    clear();
    if(DICTOOL->checkObjectExist_json(jsonDict, "messages"))
    {
        const rapidjson::Value& messages = DICTOOL->getSubDictionary_json(jsonDict, "messages");
        parseChannel(ChatType::World, messages, "world");
        parseChannel(ChatType::Private, messages, "personal");
        parseChannel(ChatType::Guild, messages, "guild");
        parseChannel(ChatType::Group, messages, "team");
        
    }
    
    if(DICTOOL->checkObjectExist_json(jsonDict, "mark"))
    {
        const rapidjson::Value& mark = DICTOOL->getSubDictionary_json(jsonDict, "mark");
        _mark->_groupId = DICTOOL->getIntValue_json(mark, "group", INVALID_VALUE);
        _mark->_wid = DICTOOL->getIntValue_json(mark, "w", INVALID_VALUE);
        _mark->_pid = DICTOOL->getIntValue_json(mark, "p", INVALID_VALUE);
        _mark->_gid = DICTOOL->getIntValue_json(mark, "g", INVALID_VALUE);
        _mark->_tid = DICTOOL->getIntValue_json(mark, "t", INVALID_VALUE);
    }
}

const vector<ChatData*>& ChatManager::getChatData(ChatType type)
{
    if (_chatData.find(type) == _chatData.end()) {
        _chatData.insert(make_pair(type, vector<ChatData*>()));
    }
    
    return _chatData.at(type);
}

void ChatManager::sendMessage(ChatType type, int contacter, const string& message)
{
    
}

void ChatManager::recieve()
{
    
}