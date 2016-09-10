//
//  ChatManager.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/1.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ChatManager.h"
#include "GameData.h"
#include "JSonUtils.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include "ChatData.h"
#include "TCPClient.h"
#include "ChatTCPClientObserver.h"

using namespace std;

static const int COMMAND_READY          (100);
static const int COMMAND_SEND_MSG       (101);
static const int COMMAND_RECEIVED_MSG   (102);

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
:_client(nullptr)
,_port(0)
{
#if true
    static const pair<string, int> host = {"123.57.221.242", 7777};
//    static const pair<string, int> host = {"192.168.31.100", 7777};
    connect(host.first, host.second);
    ready();
    
    // TODO: remove test data
#else
    _chatData.insert(make_pair(ChatType::World, vector<ChatData*>()));
    _chatData.insert(make_pair(ChatType::Mail, vector<ChatData*>()));
    _chatData.insert(make_pair(ChatType::Notice, vector<ChatData*>()));
    
    for (int i = 0; i < 10; ++i) {
        rapidjson::Document document;
        document.SetObject();
        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
        document.AddMember("time", "213123123", allocator);
        document.AddMember("channel", 1, allocator);
        document.AddMember("content", rapidjson::Value("this is a test messagethis is a test messagethis is a test messagethis is a test messagethis is a test messagethis is a test message", allocator), allocator);
        
        _chatData.at(ChatType::World).push_back(new (nothrow) ChatData(document));
    }
    
    for (int i = 0; i < 10; ++i) {
        rapidjson::Document document;
        document.SetObject();
        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
        document.AddMember("time", "213123123", allocator);
        document.AddMember("channel", 4, allocator);
        document.AddMember("content", rapidjson::Value("this is a test mail\nthis is a test mail\nthis is a test mail", allocator), allocator);
        if (i % 2 == 0) {
            document.AddMember("rewards", rapidjson::Value("101_1;102_2;103_3", allocator), allocator);
        }
        
        _chatData.at(ChatType::Mail).push_back(new (nothrow) ChatData(document));
    }
    
    for (int i = 0; i < 10; ++i) {
        rapidjson::Document document;
        document.SetObject();
        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
        document.AddMember("time", "213123123", allocator);
        document.AddMember("channel", 5, allocator);
        document.AddMember("content", rapidjson::Value("this is a test notice", allocator), allocator);
        
        _chatData.at(ChatType::Notice).push_back(new (nothrow) ChatData(document));
    }
#endif
}

ChatManager::~ChatManager()
{
    disconnect();
    clear();
}

#pragma mark - public
void ChatManager::addObserver(ChatTCPClientObserver* observer)
{
    _observers.insert(observer);
}

void ChatManager::removeObserver(ChatTCPClientObserver* observer)
{
    _observers.erase(observer);
}

void ChatManager::connect(const string& host, int port)
{
    disconnect();
    
    _host = host;
    _port = port;
    
    auto client(new (nothrow) TCPClient());
    client->setTimeoutForConnect(300);
    client->init(host, port);
    client->setResponseCallback(bind(&ChatManager::onReceivedTcpResponse, this, placeholders::_1, placeholders::_2));
    client->setReconnectCallback(bind(&ChatManager::onReconncected, this, placeholders::_1, placeholders::_2));
    _client = client;
    
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([this]() {
        for (auto observer : _observers) {
            observer->onChatTCPClientConnected();
        }
    });
}

void ChatManager::ready()
{
    sendRequest([](rapidjson::Document& document) {
        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
        document.AddMember("code", COMMAND_READY, allocator);
        document.AddMember("uid", GameData::getInstance()->getUUID(), allocator);
    });
}

void ChatManager::disconnect()
{
    if (_client) {
        _client->destroy();
        _client = nullptr;
    }
}

const vector<ChatData*>& ChatManager::getChatData(ChatType type)
{
    if (_chatData.find(type) == _chatData.end()) {
        _chatData.insert(make_pair(type, vector<ChatData*>()));
    }
    
    return _chatData.at(type);
}

void ChatManager::sendMessage(ChatType type, int contacter, const char* msg)
{
    if (msg && strlen(msg) > 0) {
        sendRequest([=](rapidjson::Document& document) {
            rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
            document.AddMember("code", COMMAND_SEND_MSG, allocator);
            document.AddMember("uid", GameData::getInstance()->getUUID(), allocator);
            document.AddMember("channel", static_cast<int>(type), allocator);
            document.AddMember("to", contacter, allocator);
            document.AddMember("content", rapidjson::Value(msg, allocator), allocator);
        });
    }
}

void ChatManager::fetchMessages()
{
    
}

#pragma mark - private
void ChatManager::reconnect()
{
    
}

void ChatManager::sendRequest(const function<void(rapidjson::Document& document)>& generator)
{
    if (_client) {
        rapidjson::Document document;
        document.SetObject();
        generator(document);
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);
        string raw = buffer.GetString();
        auto len(raw.size());
        if (len > 0) {
            auto request = new (nothrow) TCPRequest();
            request->setRequestData(raw.c_str(), len);
            _client->send(request);
        }
    }
}

void ChatManager::onReceivedTcpResponse(TCPClient* client, TCPResponse* response)
{
    if (_client != client) {
        CC_SAFE_RELEASE(response);
        return;
    }
    
    if (response) {
        rapidjson::Document document;
        document.Parse<0>(response->getResponseDataString());
        if (!document.HasParseError()) {
            const int code(JSonUtils::parse<int>(document, "code"));
            if (COMMAND_RECEIVED_MSG == code) {
                auto data = new (nothrow) ChatData(document);
                auto type(data->getType());
                if (_chatData.find(type) == _chatData.end()) {
                    _chatData.insert(make_pair(type, vector<ChatData*>()));
                }
                _chatData.at(type).push_back(data);
                
                Director::getInstance()->getScheduler()->performFunctionInCocosThread([this, data]() {
                    const bool isSelf(data->getUid() == GameData::getInstance()->getUUID());
                    for (auto observer : _observers) {
                        if (isSelf) {
                            observer->onChatTCPClientSent(data);
                        } else {
                            observer->onChatTCPClientReceived(data);
                        }
                    }
                });
            }
        }
    }
}

void ChatManager::onReconncected(TCPClient* client, TCPResponse* response)
{
    if (_client != client) {
        CC_SAFE_RELEASE(response);
        return;
    }
    
    ready();
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
