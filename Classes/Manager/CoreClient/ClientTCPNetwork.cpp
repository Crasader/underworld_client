//
//  ClientTCPNetworkProxy.cpp
//  Underworld_Client
//
//  Created by wenchengye on 16/3/22.
//
//

#include <stdlib.h>

#include "ClientTCPNetwork.h"

#include "GameSettings.h"
#include "GameModeHMM.h"
#include "cocos2d.h"
#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include "cocostudio/DictionaryHelper.h"
#include "Utils.h"
#include "DataManager.h"
#include "BinaryJsonTool.h"
#include "CoreUtils.h"
#include "UWJsonHelper.h"

#define MESSAGE_CODE_MATCH_2_S      (2)
#define MESSAGE_CODE_MATCHED_2_C      (3)
#define MESSAGE_CODE_SYNC_2_S        (4)
#define MESSAGE_CODE_SYNC_2_C        (5)
#define MESSAGE_CODE_RECONNECT_2_S   (6)
#define MESSAGE_CODE_START_2_C      (7)
#define MESSAGE_CODE_FINISH_2_S      (8)
#define MESSAGE_CODE_CANCEL_2_S      (10)
#define MESSAGE_CODE_CANCEL_2_C      (11)
#define MESSAGE_CODE_START_2_S      (12)

#define MESSAGE_KEY_CODE         ("code")
#define MESSAGE_KEY_CARDS        ("cards")
#define MESSAGE_KEY_INIT_UNITS   ("initUnits")
#define MESSAGE_KEY_UNIT_POOL    ("unitPools")
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
#define MESSAGE_KEY_NAME         ("name")
#define MESSAGE_KEY_UID          ("uid")
#define MESSAGE_KEY_BATTLE_ID    ("battleid")
#define MESSAGE_KEY_FINISHED     ("finished")

#define PROTOCOL_HEAD_LENGTH  (4)

using namespace UnderWorld::Core;
using namespace rapidjson;
using namespace cocostudio;

static bool syncMessageCompare(NetworkMessageSync* a, NetworkMessageSync* b) {
    return a->getFrame() < b->getFrame();
}

static std::string parseMatch2SMsg(
    const NetworkMessageMatch2S* msg, std::string name, int uid) {
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root(rapidjson::kObjectType);
    
    rapidjson::Value nameJson(rapidjson::kStringType);
    nameJson.SetString(name.c_str(), (int)name.size(), allocator);
    
    rapidjson::Value uidJson(rapidjson::kNumberType);
    uidJson.SetInt(uid);
    
    rapidjson::Value reqCode(rapidjson::kNumberType);
    reqCode.SetInt(MESSAGE_CODE_MATCH_2_S);
    
    rapidjson::Value cards(rapidjson::kStringType);
    std::string cardString = "";
    const vector<int>& cardNames = msg->getCards();
    for (int i = 0; i < cardNames.size(); ++i) {
        cardString.append(UnderWorldCoreUtils::to_string(cardNames[i]));
        if (i != cardNames.size() - 1) cardString.append("|");
    }
    cards.SetString(cardString.c_str(), (int)cardString.size(), allocator);
    
    rapidjson::Value initUnitsJson(rapidjson::kStringType);
    std::string initUnitsString = "";
    const GameModeHMMSetting::InitUnitList& initUnits = msg->getInitUnits();
    for (int i = 0; i < initUnits.size(); ++i) {
        initUnitsString.append(UnderWorldCoreUtils::to_string(initUnits[i].first)).append("|").append(UnderWorldCoreUtils::to_string(initUnits[i].second));
        if (i != initUnits.size() - 1) initUnitsString.append(",");
    }
    initUnitsJson.SetString(initUnitsString.c_str(), (int)initUnitsString.size(), allocator);
    
    rapidjson::Value unitPoolJson(rapidjson::kStringType);
    std::string unitPoolString = "";
    const std::vector<HMMCardSetting>& unitPool = msg->getUnitPool();
    //TODO: parse card pool string
//    for (int i = 0; i < unitPool.size(); ++i) {
//        unitPoolString.append(UnderWorldCoreUtils::to_string(unitPool[i].getUnitTypeId())).append("|")
//            .append(UnderWorldCoreUtils::to_string(unitPool[i].getLevel())).append("|")
//            .append(UnderWorldCoreUtils::to_string(unitPool[i].getQuality())).append("|")
//            .append(UnderWorldCoreUtils::to_string(unitPool[i].getTalentLevel()));
//        if (i != unitPool.size() - 1) unitPoolString.append(",");
//    }
    unitPoolJson.SetString(unitPoolString.c_str(), (int)unitPoolString.size(), allocator);
    
    root.AddMember(MESSAGE_KEY_CODE, reqCode, allocator);
    root.AddMember(MESSAGE_KEY_CARDS, cards, allocator);
    root.AddMember(MESSAGE_KEY_NAME, nameJson, allocator);
    root.AddMember(MESSAGE_KEY_UID, uidJson, allocator);
    root.AddMember(MESSAGE_KEY_INIT_UNITS, initUnitsJson, allocator);
    root.AddMember(MESSAGE_KEY_UNIT_POOL, unitPoolJson, allocator);
    
//    rapidjson::StringBuffer buffer;
//    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
//    root.Accept(writer);
//    return buffer.GetString();
    return DataManager::getInstance()->getBinaryJsonTool()->encode(root);
}

static std::string parseStart2SMsg(
                                   const NetworkMessageStart2S* msg, int uid) {
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root(rapidjson::kObjectType);
    
    rapidjson::Value uidJson(rapidjson::kNumberType);
    uidJson.SetInt(uid);
    
    rapidjson::Value reqCode(rapidjson::kNumberType);
    reqCode.SetInt(MESSAGE_CODE_START_2_S);
    
    root.AddMember(MESSAGE_KEY_CODE, reqCode, allocator);
    root.AddMember(MESSAGE_KEY_UID, uidJson, allocator);
    
    //    rapidjson::StringBuffer buffer;
    //    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    //    root.Accept(writer);
    //    return buffer.GetString();
    return DataManager::getInstance()->getBinaryJsonTool()->encode(root);
}

static std::string parseSync2SMsg(
    const NetworkMessageSync* msg, int uid) {
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root(rapidjson::kObjectType);
    
    rapidjson::Value reqCode(rapidjson::kNumberType);
    reqCode.SetInt(MESSAGE_CODE_SYNC_2_S);
    
    rapidjson::Value uidJson(rapidjson::kNumberType);
    uidJson.SetInt(uid);
    
    rapidjson::Value msgFrame(rapidjson::kNumberType);
    msgFrame.SetInt(msg->getFrame());
    
    rapidjson::Value commands(rapidjson::kArrayType);
    for (int i = 0; i < msg->getCommandCount(); ++i) {
        const UnderWorld::Core::OutsideCommand* cmd = msg->getCommand(i).first;
        int frame = msg->getCommand(i).second;
        rapidjson::Value command(rapidjson::kObjectType);
        
        rapidjson::Value commandFrame(rapidjson::kNumberType);
        commandFrame.SetInt(frame);
        command.AddMember(MESSAGE_KEY_FRAME, commandFrame, allocator);
        
        if (dynamic_cast<const UnderWorld::Core::OutsideHMMCommand*>(cmd)) {
            const UnderWorld::Core::OutsideHMMCommand* deckCmd =
                dynamic_cast<const UnderWorld::Core::OutsideHMMCommand*>(cmd);
            rapidjson::Value handIndex(rapidjson::kNumberType);
            handIndex.SetInt(deckCmd->getHandIndex());
            command.AddMember(MESSAGE_KEY_HAND_INDEX, handIndex, allocator);
            
            rapidjson::Value factionIndex(rapidjson::kNumberType);
            factionIndex.SetInt(deckCmd->getFactionIndex());
            command.AddMember(MESSAGE_KEY_FAC_INDEX, factionIndex, allocator);
            
            rapidjson::Value pos(rapidjson::kStringType);
            std::string posString = UnderWorld::Core::UnderWorldCoreUtils::to_string(deckCmd->getPos().x) + "_"
                + UnderWorld::Core::UnderWorldCoreUtils::to_string(deckCmd->getPos().y);
            pos.SetString(posString.c_str(), (int)posString.size(), allocator);
            command.AddMember(MESSAGE_KEY_POS, pos, allocator);
        }
        
        commands.PushBack(command, allocator);
    }
    
    root.AddMember(MESSAGE_KEY_CODE, reqCode, allocator);
    root.AddMember(MESSAGE_KEY_UID, uidJson, allocator);
    root.AddMember(MESSAGE_KEY_FRAME, msgFrame, allocator);
    root.AddMember(MESSAGE_KEY_COMMANDS, commands, allocator);
    
//    rapidjson::StringBuffer buffer;
//    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
//    root.Accept(writer);
//    return buffer.GetString();
    return DataManager::getInstance()->getBinaryJsonTool()->encode(root);
}

static std::string parseFinish2SMsg(const NetworkMessageFinish2S* msg, int uid) {
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root(rapidjson::kObjectType);
    
    rapidjson::Value reqCode(rapidjson::kNumberType);
    reqCode.SetInt(MESSAGE_CODE_FINISH_2_S);
    
    rapidjson::Value uidJson(rapidjson::kNumberType);
    uidJson.SetInt(uid);
    
    root.AddMember(MESSAGE_KEY_CODE, reqCode, allocator);
    root.AddMember(MESSAGE_KEY_UID, uidJson, allocator);
    
//    rapidjson::StringBuffer buffer;
//    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
//    root.Accept(writer);
//    return buffer.GetString();
    return DataManager::getInstance()->getBinaryJsonTool()->encode(root);
}

static std::string parseReconnect2SMsg(int uid, int battleid) {
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value root(rapidjson::kObjectType);
    
    rapidjson::Value reqCode(rapidjson::kNumberType);
    reqCode.SetInt(MESSAGE_CODE_RECONNECT_2_S);
    
    rapidjson::Value uidJson(rapidjson::kNumberType);
    uidJson.SetInt(uid);
    
    rapidjson::Value battleidJson(rapidjson::kNumberType);
    battleidJson.SetInt(battleid);
    
    rapidjson::Value msgFrame(rapidjson::kNumberType);
#pragma core frame
    msgFrame.SetInt(0);
    
    root.AddMember(MESSAGE_KEY_CODE, reqCode, allocator);
    root.AddMember(MESSAGE_KEY_UID, uidJson, allocator);
    root.AddMember(MESSAGE_KEY_BATTLE_ID, battleidJson, allocator);
    root.AddMember(MESSAGE_KEY_FRAME, msgFrame, allocator);
    
//    rapidjson::StringBuffer buffer;
//    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
//    root.Accept(writer);
//    return buffer.GetString();
    return DataManager::getInstance()->getBinaryJsonTool()->encode(root);
}


static void parseMatched2CMsg(const rapidjson::Value& root,
        std::vector<NetworkMessage *> &output, int& battleid) {
    NetworkMessageMatched2C* msg = new NetworkMessageMatched2C();
    
    /** 0. battle Id */
    battleid = UWJsonHelper::getIntValue_json(root, MESSAGE_KEY_BATTLE_ID, INVALID_VALUE);
    
    /** 0. map Id */
    int mapId = UWJsonHelper::getIntValue_json(root, MESSAGE_KEY_MAP_ID, 0);
    msg->setMapId(mapId);
    
    /** 1. faction count */
    FactionSetting fs;
    int factionCount = UWJsonHelper::getIntValue_json(root, MESSAGE_KEY_FAC_COUNT, 0);
    fs.setFactionCount(factionCount);
    
    /** 2. this faction */
    int thisFaction = UWJsonHelper::getIntValue_json(root, MESSAGE_KEY_FAC_INDEX, 0);
    fs.setThisFactionIndex(thisFaction);
    
    /** 3. faction control type */
    //Temp Code
    for (int i = 0; i < factionCount; ++i) {
        fs.setFactionControlType(i, kFactionControlType_Human);
    }
    
    /** 4. teams */
    if (UWJsonHelper::checkObjectExist_json(root, MESSAGE_KEY_TEAMS)) {
        std::string teamString =
            UWJsonHelper::getStringValue_json(root, MESSAGE_KEY_TEAMS);
        if (!teamString.empty()) {
            std::vector<string> teamVec;
            ::Utils::split(teamVec, teamString, "_");
            for (int i = 0; i < teamVec.size(); ++i) {
                fs.setTeam(i, atoi(teamVec[i].c_str()));
            }
        }
    }
    
    /** 5. map Indexs */
    if (UWJsonHelper::checkObjectExist_json(root, MESSAGE_KEY_MAP_INDEXS)) {
        std::string mapIndexs =
            UWJsonHelper::getStringValue_json(root, MESSAGE_KEY_MAP_INDEXS);
        if (!mapIndexs.empty()) {
            std::vector<string> mapIndexsVec;
            UnderWorldCoreUtils::split(mapIndexsVec, mapIndexs, "_");
            for (int i = 0; i < mapIndexsVec.size(); ++i) {
                fs.setMapIndex(i, atoi(mapIndexsVec[i].c_str()));
            }
        }
    }
    
    /** 6. players */
    std::vector<std::vector<int> > cardNames;
    std::vector<GameModeHMMSetting::InitUnitList> initUnits;
    std::vector<std::vector<HMMCardSetting> > unitPools;
    if (UWJsonHelper::checkObjectExist_json(root, MESSAGE_KEY_PLAYERS)
        && UWJsonHelper::getSubDictionary_json(root, MESSAGE_KEY_PLAYERS).IsArray()) {
        const rapidjson::Value& players =
            UWJsonHelper::getSubDictionary_json(root, MESSAGE_KEY_PLAYERS);
        
        cardNames.resize(players.Size());
        initUnits.resize(players.Size());
        unitPools.resize(players.Size());
        for (int i = 0; i < players.Size(); ++i) {
            const rapidjson::Value& player = players[i];
            GameContentSetting gcs;
            
            /** 6.1 player's cards */
            if (UWJsonHelper::checkObjectExist_json(player, MESSAGE_KEY_CARDS)) {
                std::string cards = UWJsonHelper::getStringValue_json(player, MESSAGE_KEY_CARDS);
                if (!cards.empty()) {
                    std::vector<std::string> cardsVes;
                    UnderWorldCoreUtils::split(cardsVes, cards, "|");
                    cardNames[i].clear();
                    for (int j = 0; j < cardsVes.size(); ++j) {
                        cardNames[i].push_back(atoi(cardsVes[j].c_str()));
                    }
                    
                }
            }
            
            /** 6.2 player's init units */
            if (UWJsonHelper::checkObjectExist_json(player, MESSAGE_KEY_INIT_UNITS)) {
                std::string initUnitsString = UWJsonHelper::getStringValue_json(player, MESSAGE_KEY_INIT_UNITS);
                if (!initUnitsString.empty()) {
                    static std::vector<std::string> initUnitsVec;
                    initUnitsVec.clear();
                    UnderWorldCoreUtils::split(initUnitsVec, initUnitsString, ",");
                    
                    for (int j = 0; j < initUnitsVec.size(); ++j) {
                        static std::vector<string> initUnitVec;
                        initUnitVec.clear();
                        UnderWorldCoreUtils::split(initUnitVec, initUnitsVec[j], "|");
                        
                        if (initUnitVec.size() == 2) {
                            initUnits[i].push_back(std::make_pair(atoi(initUnitVec[0].c_str()),
                                atoi(initUnitVec[1].c_str())));
                        }
                    }
                }
            }
            
            /** 6.3 player's unit pool */
            if (UWJsonHelper::checkObjectExist_json(player, MESSAGE_KEY_UNIT_POOL)) {
                std::string unitPoolString = UWJsonHelper::getStringValue_json(player, MESSAGE_KEY_UNIT_POOL);
                if (!unitPoolString.empty()) {
                    //TODO: parse card pool string
//                    static std::vector<std::string> unitPoolVec;
//                    unitPoolVec.clear();
//                    UnderWorldCoreUtils::split(unitPoolVec, unitPoolString, ",");
//                    
//                    for (int j = 0; j < unitPoolVec.size(); ++j) {
//                        static std::vector<std::string> unitVec;
//                        unitVec.clear();
//                        UnderWorldCoreUtils::split(unitVec, unitPoolVec[j], "|");
//                        
//                        if (unitVec.size() == 4) {
//                            UnitSetting us;
//                            us.setUnitTypeId(atoi(unitVec[0].c_str()));
//                            us.setLevel(atoi(unitVec[1].c_str()));
//                            us.setQuality(atoi(unitVec[2].c_str()));
//                            us.setTalentLevel(atoi(unitVec[3].c_str()));
//                            unitPools[i].push_back(us);
//                        }
//                    }
                }
            }
            
            
            //TODO:temp code, core and tower setting
            gcs.setFactionTypeKey("狼人族");
            UnitSetting core;
            core.setUnitTypeId(10000); //狼人基地
            core.setLevel(0);core.setQuality(0);core.setTalentLevel(0);
            gcs.setCore(core);
            
            fs.setContentSetting(gcs, i);
        }
    }
    
    msg->setFactionSetting(fs);
    msg->setCards(cardNames);
    msg->setInitUnitLists(initUnits);
    msg->setUnitPools(unitPools);
    output.push_back(msg);
}

static void parseStart2CMsg(const rapidjson::Value& root,
                              std::vector<NetworkMessage *> &output, int& battleid) {
    NetworkMessageStart2C* msg = new NetworkMessageStart2C();
    output.push_back(msg);
}

static void parseSync2CMsg(const rapidjson::Value& root,
    std::vector<NetworkMessage *> &output, int& battleid) {
    if (!UWJsonHelper::checkObjectExist_json(root, MESSAGE_KEY_START_FRAME)
        || !UWJsonHelper::checkObjectExist_json(root, MESSAGE_KEY_END_FRAME)) {
        return;
    }
    
    int startFrame = UWJsonHelper::getIntValue_json(root, MESSAGE_KEY_START_FRAME, 0);
    int endFrame = UWJsonHelper::getIntValue_json(root, MESSAGE_KEY_END_FRAME, 0);
    
    assert(GameConstants::isKeyFrame(startFrame)
        && GameConstants::isKeyFrame(endFrame)
        && startFrame <= endFrame);
    
    std::vector<NetworkMessageSync*> syncMsgs;
    for (int i = startFrame;
         i <= endFrame;
         i += GameConstants::NETWORK_KEY_FRAME_RATE) {
        syncMsgs.push_back(new NetworkMessageSync(i));
    }
    
    if (UWJsonHelper::checkObjectExist_json(root, MESSAGE_KEY_COMMANDS)
        && UWJsonHelper::getSubDictionary_json(root, MESSAGE_KEY_COMMANDS).IsArray()) {
        const rapidjson::Value& commands = UWJsonHelper::getSubDictionary_json(root, MESSAGE_KEY_COMMANDS);
        for (int i = 0; i < commands.Size(); ++i) {
            int frame = UWJsonHelper::getIntValue_json(commands[i], MESSAGE_KEY_FRAME);
            int type = UWJsonHelper::getIntValue_json(commands[i], MESSAGE_KEY_TYPE);
            
            assert(frame <= endFrame && frame >= startFrame);
            //Temp code
            int handIndex = UWJsonHelper::getIntValue_json(commands[i], MESSAGE_KEY_HAND_INDEX, 0);
            int factionIndex = UWJsonHelper::getIntValue_json(commands[i], MESSAGE_KEY_FAC_INDEX, 0);
            Coordinate32 pos;
            if (UWJsonHelper::checkObjectExist_json(commands[i], MESSAGE_KEY_POS)) {
                std::string posString = UWJsonHelper::getStringValue_json(commands[i], MESSAGE_KEY_POS);
                std::string::size_type index = posString.find("_");
                if (index != std::string::npos && index != posString.size() - 1) {
                    pos.x = atoi(posString.substr(0, posString.find("_")).c_str());
                    pos.y = atoi(posString.substr(posString.find("_") + 1).c_str());
                }
            }
            OutsideHMMCommand* cmd = new OutsideHMMCommand(handIndex,
                factionIndex, pos);
            syncMsgs[(frame - startFrame) / GameConstants::NETWORK_KEY_FRAME_RATE]->addCommand(cmd, frame);
        }
    }
    
    for (int i = 0; i < syncMsgs.size(); ++i) {
        output.push_back(syncMsgs[i]);
    }
    
    bool finished = UWJsonHelper::getBooleanValue_json(root, MESSAGE_KEY_FINISHED, false);
    if (finished && !syncMsgs.empty()) {
        syncMsgs.back()->setFinished(finished);
    }
}

ClientTCPNetwork::~ClientTCPNetwork() {
    destroyTCPClient();
    cleanSyncInstance();
    M_SAFE_DELETE(_matchMsg);
}

void ClientTCPNetwork::destroyTCPClient() {
    if (_tcpClient) {
        _tcpClient->destroy();
        _tcpClient = nullptr;
    }
    _battleid = INVALID_VALUE;
    _status = ClientStatus::Idle;
}

void ClientTCPNetwork::connect() {
    destroyTCPClient();
    _tcpClient = new TCPClient();
    _tcpClient->setTimeoutForConnect(300);
    _tcpClient->init(_host, _port);
    _tcpClient->setResponseCallback(std::bind(&ClientTCPNetwork::onReceiveTCPResponse, this, std::placeholders::_1, std::placeholders::_2));
    _tcpClient->setReconnectCallback(std::bind(&ClientTCPNetwork::onReconncected, this, std::placeholders::_1));
}

void ClientTCPNetwork::matchGame(LaunchListener* launchListener,
    const UnderWorld::Core::GameContentSetting& contentSetting,
    const std::vector<int>& cards,
    const UnderWorld::Core::GameModeHMMSetting::InitUnitList& initList,
    const vector<UnderWorld::Core::HMMCardSetting>& unitPool) {
    if (_status != ClientStatus::Idle || !_tcpClient) return;
    
    _status = ClientStatus::Matching;
    _launchListener = launchListener;
    NetworkMessageMatch2S* msg = new NetworkMessageMatch2S();
    msg->setGameContentSetting(contentSetting);
    msg->setCards(cards);
    msg->setInitUnits(initList);
    msg->setUnitPool(unitPool);
    if (_matchMsg) M_SAFE_DELETE(_matchMsg);
    _matchMsg = dynamic_cast<NetworkMessageMatch2S*>(msg->clone());
    send(msg);
}

void ClientTCPNetwork::startGame() {
    if (_status != ClientStatus::Matching || !_tcpClient) return;
    
    _status = ClientStatus::Launching;
    NetworkMessageStart2S* msg = new NetworkMessageStart2S();
    send(msg);
}

void ClientTCPNetwork::closeGame() {
    if ((_status != ClientStatus::Fighting && _status != ClientStatus::Finished)
        || !_tcpClient) return;
    
    NetworkMessageFinish2S* msg = new NetworkMessageFinish2S();
    send(msg);
}

void ClientTCPNetwork::init() {
    cleanSyncInstance();
    
    _safeFrame = _lastSafeFrame = 0;
}

void ClientTCPNetwork::sendOutsideCommand(OutsideCommand* commmd) {
    _outputCommands.push_back(commmd);
}

void ClientTCPNetwork::update(int nextFrame, std::vector<CmdFramePair>& incomeCommands) {
    updateFrame(nextFrame);
    if (GameConstants::isKeyFrame(nextFrame)) {
        updateNetworkKeyFrame(nextFrame, incomeCommands);
    }
}

int ClientTCPNetwork::getSynchronizedFrame() const {
    return _safeFrame;
}

void ClientTCPNetwork::send(NetworkMessage* msg) {
    TCPRequest* req = parseMsg2Request(msg);
    if (req) {
        _tcpClient->send(req);
    }
    CC_SAFE_DELETE(msg);
}

void ClientTCPNetwork::onReceiveTCPResponse(TCPClient* client, TCPResponse* response) {
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([=](){
        if(_tcpClient != client) {
            response->release();
            return;
        }
        
        std::vector<NetworkMessage*> msgs;
        parseResponse2Msg(response, msgs);
        response->release();
        for (int i = 0; i < msgs.size(); ++i) {
            if (M_INSTANCE_OF(msgs[i], NetworkMessageMatched2C*) && _status == ClientStatus::Matching) {
                if (_launchListener) {
                    _launchListener->onMatched(*(dynamic_cast<NetworkMessageMatched2C*>(msgs[i])));
                }
                _status = ClientStatus::Launching;
            } else if (M_INSTANCE_OF(msgs[i], NetworkMessageStart2C*) && _status == ClientStatus::Launching) {
                if (_launchListener) {
                    _launchListener->onLaunched(*(dynamic_cast<NetworkMessageStart2C*>(msgs[i])));
                }
                _status = ClientStatus::Fighting;
            } else if (M_INSTANCE_OF(msgs[i], NetworkMessageSync*) && _status == ClientStatus::Fighting) {
                _incomeNetworkMessages.push_back(msgs[i]->clone());
                if (dynamic_cast<NetworkMessageSync*>(msgs[i])->isFinished()) {
                    _status = ClientStatus::Finished;
                }
            }
        }
        
        for (int i = 0; i < msgs.size(); ++i) {
            delete msgs[i];
        }
    });
}


void ClientTCPNetwork::onReconncected(TCPClient* client) {
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([=](){
        if(_tcpClient != client) return;
        CCLOG("[server][%s]", __FUNCTION__);
        //TODO when game finish, return
        if (_status == ClientStatus::Finished) {
            return;
        }
        
        if (_status == ClientStatus::Idle) {
            //TODO need to resend battle user info
        } else if (_status == ClientStatus::Fighting) {
            send(new NetworkMessageReconnect2S());
        } else if (_status == ClientStatus::Matching) {
            if (_matchMsg) send(_matchMsg->clone());
        }
    });
}

TCPRequest* ClientTCPNetwork::parseMsg2Request(
    const NetworkMessage* msg) {
    std::string reqContent = "";
    if (dynamic_cast<const NetworkMessageMatch2S*>(msg)) {
        const NetworkMessageMatch2S* l2s =
            dynamic_cast<const NetworkMessageMatch2S*>(msg);
        reqContent = parseMatch2SMsg(l2s, _name, _uid);
    } else if (dynamic_cast<const NetworkMessageStart2S*>(msg)) {
        const NetworkMessageStart2S* l2s =
        dynamic_cast<const NetworkMessageStart2S*>(msg);
        reqContent = parseStart2SMsg(l2s, _uid);
    } else if (dynamic_cast<const NetworkMessageSync*>(msg)) {
        const NetworkMessageSync* sync =
            dynamic_cast<const NetworkMessageSync*>(msg);
        reqContent = parseSync2SMsg(sync, _uid);
    } else if (dynamic_cast<const NetworkMessageFinish2S*>(msg)) {
        const NetworkMessageFinish2S* f2s = dynamic_cast<const NetworkMessageFinish2S*>(msg);
        reqContent = parseFinish2SMsg(f2s, _uid);
    } else if (dynamic_cast<const NetworkMessageReconnect2S*>(msg)) {
        reqContent = parseReconnect2SMsg(_uid, _battleid);
    }
    
    TCPRequest* ret = nullptr;
    if (!reqContent.empty()) {
        ret = new TCPRequest();
        ret->setRequestData(reqContent.c_str(), reqContent.size());
    }
    return ret;
}

void ClientTCPNetwork::parseResponse2Msg(
    const TCPResponse* response,
    std::vector<NetworkMessage*> &output) {
    ;
    
    string data;
    response->getResponseDataString(data);
    
//    CCLOG("[server]%s", data.c_str());
    
    rapidjson::Document document;
//    document.Parse<rapidjson::kParseNoFlags>(data.c_str());
    bool success = DataManager::getInstance()->getBinaryJsonTool()->decode(data, document);
    if (!success) {
        return;
    }
    
#if COCOS2D_DEBUG
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    CCLOG("[server]%s", buffer.GetString());
#endif
    
    if (!UWJsonHelper::checkObjectExist_json(document, MESSAGE_KEY_CODE)) {
        return;
    }
    
    int respCode = UWJsonHelper::getIntValue_json(document, MESSAGE_KEY_CODE);
    
    if (respCode == MESSAGE_CODE_MATCHED_2_C) {
        parseMatched2CMsg(document, output, _battleid);
    } else if (respCode == MESSAGE_CODE_START_2_C) {
        parseStart2CMsg(document, output, _battleid);
    } else if (respCode == MESSAGE_CODE_SYNC_2_C) {
        parseSync2CMsg(document, output, _battleid);
    }
}

void ClientTCPNetwork::cleanSyncInstance() {
    for (int i = 0; i < _outputCommands.size(); ++i) {
        delete _outputCommands[i];
    }
    _outputCommands.clear();
    
    for (int i = 0; i < _incomeNetworkMessages.size(); ++i) {
        delete _incomeNetworkMessages[i];
    }
    _incomeNetworkMessages.clear();
}
void ClientTCPNetwork::updateFrame(int nextFrame) {
    if (!_outputCommands.empty()) {
        flushOutputCommands(_safeFrame);
    }
}

void ClientTCPNetwork::updateNetworkKeyFrame(int nextFrame,
    std::vector<CmdFramePair>& incomeCommands) {
    _lastSafeFrame = _safeFrame;
    
    /** proccess sync messages */
    std::vector<NetworkMessageSync*> syncMsgs;
    for (int i = 0; i < _incomeNetworkMessages.size(); ++i) {
        NetworkMessageSync* syncMsg =
        dynamic_cast<NetworkMessageSync*>(_incomeNetworkMessages[i]);
        if (syncMsg) {
            syncMsgs.push_back(syncMsg);
        }
    }
    
    std::sort(syncMsgs.begin(), syncMsgs.end(), syncMessageCompare);
    
    for (int i = 0; i < syncMsgs.size(); ++i) {
        assert(GameConstants::isKeyFrame(syncMsgs[i]->getFrame()));
        if (syncMsgs[i]->getFrame() == GameConstants::nextKeyFrame(_safeFrame)) {
            _safeFrame = syncMsgs[i]->getFrame();
            for (int j = 0; j < syncMsgs[i]->getCommandCount(); ++j) {
                const CmdFramePair commandPair = syncMsgs[i]->getCommand(j);
                incomeCommands.push_back(std::make_pair(commandPair.first->clone(),
                    commandPair.second));
            }
        }
    }
    
    assert(_safeFrame == 0 || GameConstants::isKeyFrame(_safeFrame));
    
    if (_safeFrame > _lastSafeFrame) {
        /** sync with server */
        NetworkMessageSync* msg = new NetworkMessageSync(_safeFrame);
        send(msg);
    }
    
    for (int i = 0; i < _incomeNetworkMessages.size(); ++i) {
        delete _incomeNetworkMessages[i];
    }
    _incomeNetworkMessages.clear();
}

void ClientTCPNetwork::flushOutputCommands(int frame) {
    NetworkMessageSync* msg = new NetworkMessageSync(frame);
    for (int i = 0; i < _outputCommands.size(); ++i) {
        msg->addCommand(_outputCommands[i], frame);
    }
    send(msg);
    _outputCommands.clear();
}

NetworkMessageSync::~NetworkMessageSync() {
    for (int i = 0; i < _commands.size(); ++i) {
        delete _commands[i].first;
    }
    _commands.clear();
}

NetworkMessage* NetworkMessageSync::clone() const {
    NetworkMessageSync* instance = new NetworkMessageSync(this->_frame);
    for (int i = 0; i < this->_commands.size(); ++i) {
        instance->_commands.push_back(
            std::make_pair(this->_commands[i].first->clone(),
            this->_commands[i].second));
    }
    return instance;
}



