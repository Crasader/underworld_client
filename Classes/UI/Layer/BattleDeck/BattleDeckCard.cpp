//
//  BattleDeckCard.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/11.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "BattleDeckCard.h"

using namespace std;

BattleDeckCard* BattleDeckCard::create(const string& name)
{
    BattleDeckCard *ret = new (nothrow) BattleDeckCard();
    if (ret && ret->init(name)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

BattleDeckCard::BattleDeckCard()
:_observer(nullptr) {}

BattleDeckCard::~BattleDeckCard()
{
    removeAllChildren();
}

bool BattleDeckCard::init(const string& name)
{
    if (Node::init())
    {
        return true;
    }
    
    return false;
}

void BattleDeckCard::registerObserver(BattleDeckCardObserver *observer)
{
    _observer = observer;
}

void BattleDeckCard::update(const string& name)
{
    
}

const string& BattleDeckCard::getCardName() const
{
    return _cardName;
}
