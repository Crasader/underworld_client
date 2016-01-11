//
//  PieceManager.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "PieceManager.h"
#include "cocostudio/CocoStudio.h"
#include "Utils.h"
#include "SoldierPieceData.h"
#include "HeroPieceData.h"

using namespace std;
using namespace cocostudio;

PieceManager::PieceManager()
{
}

PieceManager::~PieceManager()
{
    
}

void PieceManager::init(const rapidjson::Value& jsonDict)
{
    
}

void PieceManager::compose(int id, int count)
{
    if (_soldierPieces.find(id) != _soldierPieces.end()) {
        const int cnt = _soldierPieces.at(id);
        if (cnt >= count) {
            _soldierPieces.at(id) = cnt - count;
        }
    } else if (_heroPieces.find(id) != _heroPieces.end()) {
        const int cnt = _heroPieces.at(id);
        if (cnt >= count) {
            _heroPieces.at(id) = cnt - count;
        }
    } else {
        assert(false);
    }
}

void PieceManager::sell(int id, int count)
{
    if (_soldierPieces.find(id) != _soldierPieces.end()) {
        const int cnt = _soldierPieces.at(id);
        if (cnt >= count) {
            _soldierPieces.at(id) = cnt - count;
        }
    } else if (_heroPieces.find(id) != _heroPieces.end()) {
        const int cnt = _heroPieces.at(id);
        if (cnt >= count) {
            _heroPieces.at(id) = cnt - count;
        }
    } else {
        assert(false);
    }
}

int PieceManager::getPieceCount(int id)
{
    if (_soldierPieces.find(id) != _soldierPieces.end()) {
        return _soldierPieces.at(id);
    } else if (_heroPieces.find(id) != _heroPieces.end()) {
        return _heroPieces.at(id);
    }
    
    return 0;
}
