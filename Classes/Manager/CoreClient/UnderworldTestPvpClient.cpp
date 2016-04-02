//
//  UnderworldTestPvpClient.cpp
//  Underworld_Client
//
//  Created by Mac on 16/3/29.
//
//

#include "UnderworldTestPvpClient.h"
#include "ClientTCPNetworkProxy.h"
#include "cocos2d.h"
#include "Commander.h"
#include "Game.h"
#include "World.h"

//static const std::string host = "123.57.221.242";
//static const int port = 8888;

static const std::string host = "192.168.31.139";
static const int port = 8080;


UnderworldTestPvpClient::UnderworldTestPvpClient()
: _playerA(nullptr)
, _playerB(nullptr)
, _sch(nullptr) {
}

void UnderworldTestPvpClient::init(UnderWorld::Core::AbstractRender* render) {
    _sch = new GameScheduler();
    ClientTCPNetworkProxy* proxyA = new ClientTCPNetworkProxy(host, port, "playerA", 1);
    _playerA = new UnderworldClient("playerA", proxyA, _sch, render);
    ClientTCPNetworkProxy* proxyB = new ClientTCPNetworkProxy(host, port, "playerB", 2);
    _playerB = new UnderworldClient("PlayerB", proxyB, _sch, nullptr);
}

void UnderworldTestPvpClient::startTest(const UnderWorld::Core::GameContentSetting &setting) {
    _playerA->launchPvp(setting);
    Director::getInstance()->getScheduler()->schedule([this, setting](float f) {
        _playerB->launchPvp(setting);
    }, this, 0.f, 0, 1.f, false, "under_world_test_launch");
    
    Director::getInstance()->getScheduler()->schedule([this](float f) {
        UnderWorld::Core::Game& game = _playerB->getTerminal().getGame();
        int thisFactionIndex = game.getWorld()->getThisFactionIndex();
        UnderWorld::Core::Faction* thisFaction = game.getWorld()->getFaction(thisFactionIndex);
        UnderWorld::Core::Coordinate pos = thisFaction->getPuttingArea()._origin +
            UnderWorld::Core::Coordinate(thisFaction->getPuttingArea()._width / 4,
                thisFaction->getPuttingArea()._height / 4);
        UnderWorld::Core::OutsideDeckCommand* deckCommand =
            new UnderWorld::Core::OutsideDeckCommand(0, thisFactionIndex, pos);
        game.getCommander()->addCommandFromLocal(deckCommand);
    }, this, 10.f, CC_REPEAT_FOREVER, 10.f, false, "under_world_test_put");
}

void UnderworldTestPvpClient::endTest() {
    _playerA->quit();
    _playerB->quit();
}