//
//  DeckManager.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/11.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "DeckManager.h"
#include "cocostudio/CocoStudio.h"
#include "UserDefaultsDataManager.h"
#include "DeckData.h"
#include "CardSimpleData.h"

using namespace std;
using namespace cocostudio;

static inline string getDeckKey(int idx)
{ return cocos2d::StringUtils::format("deck_%d", idx); }
static const string defaultDeckKey("default_deck");

static DeckManager* s_pInstance(nullptr);
DeckManager* DeckManager::getInstance()
{
    if (!s_pInstance) {
        s_pInstance = new (nothrow) DeckManager();
    }
    
    return s_pInstance;
}

void DeckManager::purge()
{
    if (s_pInstance) {
        CC_SAFE_DELETE(s_pInstance);
    }
}

DeckManager::DeckManager()
:_defaultId(0)
{
    for (int i = 0; i < MaxCount; ++i) {
        const auto& string = UserDefaultsDataManager::getStringForKey(getDeckKey(i).c_str(), "");
        if (string.size() > 0) {
            auto data = new (nothrow) DeckData(string);
            _decks.insert(make_pair(i, data));
        }
    }
}

DeckManager::~DeckManager()
{
    for (int i = 0; i < _decks.size(); ++i) {
        CC_SAFE_DELETE(_decks.at(i));
    }
    
    for (auto iter = begin(_foundCards); iter != end(_foundCards); ++iter) {
        auto data = *iter;
        CC_SAFE_DELETE(data);
    }
}

#pragma mark - public
void DeckManager::parse(const rapidjson::Value& jsonDict)
{
    
}

int DeckManager::getDefaultDeckId() const
{
    return _defaultId;
}

void DeckManager::setDefaultDeckId(int value)
{
    _defaultId = value;
    UserDefaultsDataManager::setIntegerForKey(defaultDeckKey.c_str(), value);
    UserDefaultsDataManager::flush();
}

DeckData* DeckManager::getDeckData(int idx) const
{
    if (_decks.size() > idx) {
        return _decks.at(idx);
    }
    
    return nullptr;
}

void DeckManager::setDeckData(int idx, const DeckData* data)
{
    if (idx < MaxCount && data) {
        auto d = getDeckData(idx);
        if (d) {
            d->clone(data);
        } else {
            _decks.insert(make_pair(idx, new (nothrow) DeckData(data)));
        }
    }
}

void DeckManager::saveDeckData(int idx)
{
    if (_decks.size() > idx) {
        string output;
        auto data = _decks.at(idx);
        data->serialize(output);
        UserDefaultsDataManager::setStringForKey(getDeckKey(idx).c_str(), output);
        UserDefaultsDataManager::flush();
    }
}

DeckData* DeckManager::getDefaultDeckData() const
{
    return getDeckData(_defaultId);
}

void DeckManager::saveDefaultDeckData()
{
    saveDeckData(_defaultId);
}

const set<CardSimpleData*>& DeckManager::getFoundCards() const
{
    return _foundCards;
}

const set<string>& DeckManager::getUnfoundCards() const
{
    return _unfoundCards;
}
