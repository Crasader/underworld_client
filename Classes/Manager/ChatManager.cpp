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

using namespace std;
using namespace cocostudio;

ChatManager::ChatManager()
{
}

ChatManager::~ChatManager()
{
    
}

void ChatManager::init(const rapidjson::Document& jsonDict)
{
    
}

const vector<const ChatData*>& ChatManager::getChatData(ChatType type)
{
    if (_chatData.find(type) == _chatData.end()) {
        _chatData.insert(make_pair(type, vector<const ChatData*>()));
    }
    
    return _chatData.at(type);
}