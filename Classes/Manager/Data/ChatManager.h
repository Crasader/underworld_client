//
//  ChatManager.h
//  Underworld_Client
//
//  Created by Andy on 15/12/1.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ChatManager_h
#define ChatManager_h

#include "json/document.h"
#include "CocosGlobal.h"
#include <vector>
#include <map>

class ChatData;

class ChatManager
{
public:
    ChatManager();
    virtual ~ChatManager();
    
    void init(const rapidjson::Value& jsonDict);
    const std::vector<const ChatData*>& getChatData(ChatType type);
    
    void sendMessage(ChatType type, int contacter, const std::string& message);
    
protected:
    M_DISALLOW_COPY_AND_ASSIGN(ChatManager);
    
private:
    std::map<ChatType, std::vector<const ChatData*>> _chatData;
};

#endif /* ChatManager_h */
