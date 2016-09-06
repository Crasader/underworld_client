//
//  ChatManager.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/1.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ChatManager.h"
#include "JSonUtils.h"
#include "ChatData.h"
#include "ChatMark.h"

using namespace std;

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
    if (JSonUtils::isExist(messages, key)) {
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
    if(JSonUtils::isExist(jsonDict, "messages"))
    {
        const rapidjson::Value& messages = DICTOOL->getSubDictionary_json(jsonDict, "messages");
        parseChannel(ChatType::World, messages, "world");
        parseChannel(ChatType::Private, messages, "personal");
        parseChannel(ChatType::Guild, messages, "guild");
        parseChannel(ChatType::Group, messages, "team");
        
    }
    
    if(JSonUtils::isExist(jsonDict, "mark"))
    {
        const auto& mark = DICTOOL->getSubDictionary_json(jsonDict, "mark");
        JSonUtils::parse(_mark->_groupId, mark, "group", INVALID_VALUE);
        JSonUtils::parse(_mark->_wid, mark, "w", INVALID_VALUE);
        JSonUtils::parse(_mark->_pid, mark, "p", INVALID_VALUE);
        JSonUtils::parse(_mark->_gid, mark, "g", INVALID_VALUE);
        JSonUtils::parse(_mark->_tid, mark, "t", INVALID_VALUE);
    }
}

const vector<ChatData*>& ChatManager::getChatData(ChatType type)
{
    if (_chatData.find(type) == _chatData.end()) {
        _chatData.insert(make_pair(type, vector<ChatData*>()));
    }
    
    return _chatData.at(type);
}

void ChatManager::sendMessage(ChatType type, int contacter, const char* msg, const function<void(const char* msg)>& callback)
{
    if (msg && strlen(msg) > 0) {
        if (_chatData.find(type) == _chatData.end()) {
            _chatData.insert(make_pair(type, vector<ChatData*>()));
        }
        
        auto& vec(_chatData.at(type));
        // TODO: remove test code
        {
            rapidjson::Document document;
            document.SetObject();
            rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
            document.AddMember("uid", 1, allocator);
            document.AddMember("user", rapidjson::Value("self", allocator), allocator);
            document.AddMember("time", 123456, allocator);
            document.AddMember("content", rapidjson::Value(msg, allocator), allocator);
            
            vec.push_back(new (nothrow) ChatData(type, document));
        }
        
        if (callback) {
            callback(msg);
        }
    }
}

void ChatManager::recieve()
{
    
}