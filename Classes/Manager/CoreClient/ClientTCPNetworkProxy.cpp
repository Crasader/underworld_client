//
//  ClientTCPNetworkProxy.cpp
//  Underworld_Client
//
//  Created by wenchengye on 16/3/22.
//
//

#include <stdlib.h>

#include "GameSettings.h"
#include "ClientTCPNetworkProxy.h"
#include "NetworkMessage.h"
#include "UnderworldClient.h"
#include "cocos2d.h"
#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include "cocostudio/DictionaryHelper.h"
#include "Utils.h"

#define MESSAGE_CODE_LAUNCH_2_S (2)
#define MESSAGE_CODE_LAUNCH_2_C (3)
#define MESSAGE_CODE_SYNC_2_S   (4)
#define MESSAGE_CODE_SYNE_2_C   (5)

#define MESSAGE_KEY_CODE         ("code")
#define MESSAGE_KEY_CARDS        ("cards")
#define MESSAGE_KEY_FRAME        ("frame")
#define MESSAGE_KEY_COMMANDS     ("commands")
#define MESSAGE_KEY_TYPE         ("type")
#define MESSAGE_KEY_HAND_INDEX   ("handIndex")
#define MESSAGE_KEY_FAC_INDEX    ("factionIndex")
#define MESSAGE_KEY_POS          ("pos")
#define MESSAGE_KEY_PLAYERS      ("players")
#define MESSAGE_KEY_FAC_COUNT    ("factionCount")
#define MESSAGE_KEY_MAP_INDEXS   ("mapIndexs")
#define MESSAGE_KEY_TEAMS        ("teams")
#define MESSAGE_KEY_MAP_ID       ("mapId")
#define MESSAGE_KEY_START_FRAME  ("startFrame")
#define MESSAGE_KEY_END_FRAME    ("endFrame")

#define PROTOCOL_HEAD_LENGTH  (4)

using namespace UnderWorld::Core;
using namespace rapidjson;
using namespace cocostudio;

static std::string parseLaunch2SMsg(
    const NetworkMessageLaunch2S* msg) {
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root(rapidjson::kObjectType);
    
    rapidjson::Value reqCode(rapidjson::kNumberType);
    reqCode.SetInt(MESSAGE_CODE_LAUNCH_2_S);
    
    rapidjson::Value cards(rapidjson::kStringType);
    std::string cardString = "";
    const vector<UnderWorld::Core::CardSetting>& cardSettings =
        msg->getGameContentSetting().geCardSetting();
    for (int i = 0; i < cardSettings.size(); ++i) {
        cardString.append(cardSettings[i].getCardTypeName());
        if (i != cardString.size() - 1) {
            cardString.append("|");
        }
    }
    cards.SetString(cardString.c_str(), (int)cardString.size(), allocator);
    
    root.AddMember(MESSAGE_KEY_CODE, reqCode, allocator);
    root.AddMember(MESSAGE_KEY_CARDS, cards, allocator);
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    root.Accept(writer);
    return buffer.GetString();
}

static std::string parseSync2SMsg(
    const UnderWorld::Core::NetworkMessageSync* msg) {
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root(rapidjson::kObjectType);
    
    rapidjson::Value reqCode(rapidjson::kNumberType);
    reqCode.SetInt(MESSAGE_CODE_SYNC_2_S);
    
    rapidjson::Value msgFrame(rapidjson::kNumberType);
    msgFrame.SetInt(msg->getFrame());
    
    rapidjson::Value commands(rapidjson::kArrayType);
    for (int i = 0; i < msg->getCommandCount(); ++i) {
        const UnderWorld::Core::OutsideCommand* cmd = msg->getCommand(i);
        rapidjson::Value command(rapidjson::kObjectType);
        
        rapidjson::Value commandFrame(rapidjson::kNumberType);
        commandFrame.SetInt(cmd->getFrame());
        command.AddMember(MESSAGE_KEY_FRAME, commandFrame, allocator);
        
        if (dynamic_cast<const UnderWorld::Core::OutsideDeckCommand*>(cmd)) {
            const UnderWorld::Core::OutsideDeckCommand* deckCmd =
                dynamic_cast<const UnderWorld::Core::OutsideDeckCommand*>(cmd);
            rapidjson::Value handIndex(rapidjson::kNumberType);
            handIndex.SetInt(deckCmd->getHandIndex());
            command.AddMember(MESSAGE_KEY_HAND_INDEX, handIndex, allocator);
            
            rapidjson::Value factionIndex(rapidjson::kNumberType);
            factionIndex.SetInt(deckCmd->getFactionIndex());
            command.AddMember(MESSAGE_KEY_FAC_INDEX, factionIndex, allocator);
            
            rapidjson::Value pos(rapidjson::kStringType);
            std::string posString = std::to_string(deckCmd->getPos().x) + "_"
                + std::to_string(deckCmd->getPos().y);
            pos.SetString(posString.c_str(), (int)posString.size(), allocator);
            command.AddMember(MESSAGE_KEY_POS, pos, allocator);
        }
        
        commands.PushBack(command, allocator);
    }
    
    root.AddMember(MESSAGE_KEY_CODE, reqCode, allocator);
    root.AddMember(MESSAGE_KEY_FRAME, msgFrame, allocator);
    root.AddMember(MESSAGE_KEY_COMMANDS, commands, allocator);
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    root.Accept(writer);
    return buffer.GetString();
}

static void parseLaunch2CMsg(const rapidjson::Value& root,
        std::vector<UnderWorld::Core::NetworkMessage *> &output) {
    NetworkMessageLaunch2C* msg = new NetworkMessageLaunch2C();
    
    /** 0. map Id */
    int mapId = cocostudio::DICTOOL->getIntValue_json(root, MESSAGE_KEY_MAP_ID, 0);
    msg->setMapId(mapId);
    
    /** 1. faction count */
    FactionSetting fs;
    int factionCount = cocostudio::DICTOOL->getIntValue_json(root, MESSAGE_KEY_FAC_COUNT, 0);
    fs.setFactionCount(factionCount);
    
    /** 2. this faction */
    int thisFaction = cocostudio::DICTOOL->getIntValue_json(root, MESSAGE_KEY_FAC_INDEX, 0);
    fs.setThisFactionIndex(thisFaction);
    
    /** 3. faction control type */
    //Temp Code
    for (int i = 0; i < factionCount; ++i) {
        fs.setFactionControlType(i, kFactionControlType_Human);
    }
    
    /** 4. teams */
    if (cocostudio::DICTOOL->checkObjectExist_json(root, MESSAGE_KEY_TEAMS)) {
        std::string teamString =
            cocostudio::DICTOOL->getStringValue_json(root, MESSAGE_KEY_TEAMS);
        if (!teamString.empty()) {
            std::vector<string> teamVec;
            Utils::split(teamVec, teamString, "_");
            for (int i = 0; i < teamVec.size(); ++i) {
                fs.setTeam(i, atoi(teamVec[i].c_str()));
            }
        }
    }
    
    /** 5. map Indexs */
    if (cocostudio::DICTOOL->checkObjectExist_json(root, MESSAGE_KEY_MAP_INDEXS)) {
        std::string mapIndexs =
            cocostudio::DICTOOL->getStringValue_json(root, MESSAGE_KEY_MAP_INDEXS);
        if (!mapIndexs.empty()) {
            std::vector<string> mapIndexsVec;
            Utils::split(mapIndexsVec, mapIndexs, "_");
            for (int i = 0; i < mapIndexsVec.size(); ++i) {
                fs.setMapIndex(i, atoi(mapIndexsVec[i].c_str()));
            }
        }
    }
    
    /** 6. players */
    if (DICTOOL->checkObjectExist_json(root, MESSAGE_KEY_PLAYERS)) {
        const Value& players =
            DICTOOL->getSubDictionary_json(root, MESSAGE_KEY_PLAYERS);
        for (int i = 0; i < players.Size(); ++i) {
            const Value& player = players[i];
            GameContentSetting gcs;
            
            /** 6.1 player's cards */
            if (DICTOOL->checkObjectExist_json(player, MESSAGE_KEY_CARDS)) {
                std::string cards = DICTOOL->getStringValue_json(player, MESSAGE_KEY_CARDS);
                if (!cards.empty()) {
                    std::vector<CardSetting> cardSettings;
                    std::vector<string> cardsVec;
                    Utils::split(cardsVec, cards, "|");
                    for (int j = 0; j < cardsVec.size(); ++j) {
                        CardSetting cs;
                        cs.setCardTypeName(cardsVec[j]);
                        
                        //Temp code
                        cs.setLevel(0);
                        cs.setQuality(0);
                        cs.setTalentLevel(0);
                        cardSettings.push_back(cs);
                    }
                    gcs.setCards(cardSettings);
                }
            }
            
            fs.setContentSetting(gcs, i);
        }
    }
    
    msg->setFactionSetting(fs);
    output.push_back(msg);
}

static void parseSync2CMsg(const rapidjson::Value& root,
    std::vector<NetworkMessage *> &output) {
    if (!DICTOOL->checkObjectExist_json(root, MESSAGE_KEY_START_FRAME)
        || !DICTOOL->checkObjectExist_json(root, MESSAGE_KEY_END_FRAME)) {
        return;
    }
    
    int startFrame = DICTOOL->getIntValue_json(root, MESSAGE_KEY_START_FRAME, 0);
    int endFrame = DICTOOL->getIntValue_json(root, MESSAGE_KEY_END_FRAME, 0);
    
    assert(startFrame % GameConstants::NETWORK_KEY_FRAME_RATE == 0
        && endFrame % GameConstants::NETWORK_KEY_FRAME_RATE == 0
        && startFrame < endFrame);
    
    std::vector<NetworkMessageSync*> syncMsgs;
    for (int i = startFrame;
         i <= endFrame;
         i += GameConstants::NETWORK_KEY_FRAME_RATE) {
        syncMsgs.push_back(new NetworkMessageSync(i));
    }
    
    if (DICTOOL->checkObjectExist_json(root, MESSAGE_KEY_COMMANDS)) {
        const Value& commands = DICTOOL->getSubDictionary_json(root, MESSAGE_KEY_COMMANDS);
        for (int i = 0; i < commands.Size(); ++i) {
            int frame = DICTOOL->getIntValue_json(commands[i], MESSAGE_KEY_FRAME);
            int type = DICTOOL->getIntValue_json(commands[i], MESSAGE_KEY_TYPE);
            
            assert(frame <= endFrame && frame >= startFrame);
            //Temp code
            int handIndex = DICTOOL->getIntValue_json(commands[i], MESSAGE_KEY_HAND_INDEX, 0);
            int factionIndex = DICTOOL->getIntValue_json(commands[i], MESSAGE_KEY_FAC_INDEX, 0);
            Coordinate pos;
            if (DICTOOL->checkObjectExist_json(root, MESSAGE_KEY_POS)) {
                std::string posString = DICTOOL->getStringValue_json(root, MESSAGE_KEY_POS);
                pos.x = atoi(posString.substr(0, posString.find("_")).c_str());
                pos.y = atoi(posString.substr(posString.find("_") + 1).c_str());
            }
            OutsideDeckCommand* cmd = new OutsideDeckCommand(handIndex,
                factionIndex, pos);
            syncMsgs[(frame - startFrame) / GameConstants::NETWORK_KEY_FRAME_RATE]->addCommand(cmd);
        }
    }
    
    for (int i = 0; i < syncMsgs.size(); ++i) {
        output.push_back(syncMsgs[i]);
    }
}

ClientTCPNetworkProxy::~ClientTCPNetworkProxy() {
    std::list<NetworkMessage*>::iterator iter = _incomeMessage.begin();
    for (; iter != _incomeMessage.end(); ++iter) {
        delete *iter;
    }
    _incomeMessage.clear();
}

void ClientTCPNetworkProxy::connect() {
    cocos2d::network::TCPClient::getInstance()->init(_host, _port);
}

void ClientTCPNetworkProxy::disconnect() {
    
}

bool ClientTCPNetworkProxy::isConnected() {
    return true;
}

void ClientTCPNetworkProxy::send(UnderWorld::Core::NetworkMessage* msg) {
    cocos2d::network::TCPRequest* req = parseMsg2Request(msg);
    if (req) {
        cocos2d::network::TCPClient::getInstance()->send(req);
    }
    CC_SAFE_DELETE(msg);
}

UnderWorld::Core::NetworkMessage* ClientTCPNetworkProxy::pop() {
    //TODO: check thread
    UnderWorld::Core::NetworkMessage* ret = nullptr;
    if (!_incomeMessage.empty()) {
        ret = _incomeMessage.front();
        _incomeMessage.pop_front();
    }
    return ret;
}

void ClientTCPNetworkProxy::onReceiveTCPResponse(cocos2d::network::TCPResponse* response) {
    response->retain();
    std::function<void ()> call = [response, this](){
        std::vector<UnderWorld::Core::NetworkMessage*> msgs;
        parseResponse2Msg(response, msgs);
        for (int i = 0; i < msgs.size(); ++i) {
            _incomeMessage.push_back(msgs[i]);
        }
        response->release();
    };
    cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread(call);
}

cocos2d::network::TCPRequest* ClientTCPNetworkProxy::parseMsg2Request(
    const UnderWorld::Core::NetworkMessage* msg) {
    std::string reqContent = "";
    if (dynamic_cast<const NetworkMessageLaunch2S*>(msg)) {
        const NetworkMessageLaunch2S* l2s =
            dynamic_cast<const NetworkMessageLaunch2S*>(msg);
        reqContent = parseLaunch2SMsg(l2s);
    } else if (dynamic_cast<const UnderWorld::Core::NetworkMessageSync*>(msg)) {
        const UnderWorld::Core::NetworkMessageSync* sync =
            dynamic_cast<const UnderWorld::Core::NetworkMessageSync*>(msg);
        reqContent = parseSync2SMsg(sync);
    }
    
    cocos2d::network::TCPRequest* ret = nullptr;
    if (!reqContent.empty()) {
        ret = new cocos2d::network::TCPRequest();
        std::string data;
        int32_t contentLen = (int32_t)reqContent.size();
        data.assign((char*)(&contentLen), PROTOCOL_HEAD_LENGTH);
        data.append(reqContent);
        ret->setRequestData(data.c_str(), data.size());
    }
    return ret;
}

void ClientTCPNetworkProxy::parseResponse2Msg(
    const cocos2d::network::TCPResponse* response,
    std::vector<UnderWorld::Core::NetworkMessage *> &output) {
    if (response->getResponseData()->size() <= 4) return;
    
    int32_t* dataLen = (int32_t*)response->getResponseDataString();
    const char* data = response->getResponseDataString() + PROTOCOL_HEAD_LENGTH;
    
    assert(strlen(data) == (size_t)dataLen);
    
    rapidjson::Document document;
    document.Parse<rapidjson::kParseNoFlags>(data);
    
    if (!cocostudio::DICTOOL->checkObjectExist_json(document, MESSAGE_KEY_CODE)) {
        return;
    }
    
    int respCode = cocostudio::DICTOOL->getIntValue_json(document, MESSAGE_KEY_CODE);
    
    if (respCode == MESSAGE_CODE_LAUNCH_2_C) {
        parseLaunch2CMsg(document, output);
    } else if (respCode == MESSAGE_CODE_SYNE_2_C) {
        parseSync2CMsg(document, output);
    }
}




