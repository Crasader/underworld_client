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
#include <unordered_set>
#include <functional>

class ChatData;
class TCPClient;
class TCPResponse;
class ChatTCPClientObserver;

class ChatManager
{
public:
    static ChatManager* getInstance();
    static void purge();
    
    void addObserver(ChatTCPClientObserver* observer);
    void removeObserver(ChatTCPClientObserver* observer);
    
    void connect(const std::string& host, int port);
    void ready();
    void disconnect();
    
    const std::vector<ChatData*>& getChatData(ChatType type);
    
    void sendMessage(ChatType type, int contacter, const char* msg);
    void fetchMessages();
    
private:
    ChatManager();
    virtual ~ChatManager();
    M_DISALLOW_COPY_AND_ASSIGN(ChatManager);
    
    void reconnect();
    void sendRequest(const std::function<void(rapidjson::Document& document)>& generator);
    void onReceivedTcpResponse(TCPClient* client, TCPResponse* response);
    void onReconncected(TCPClient* client, TCPResponse* response);
    
    void clear();
    
private:
    std::map<ChatType, std::vector<ChatData*>> _chatData;
    TCPClient* _client;
    std::unordered_set<ChatTCPClientObserver*> _observers;
    std::string _host;
    int _port;
};

#endif /* ChatManager_h */
