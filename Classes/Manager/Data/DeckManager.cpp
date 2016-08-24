//
//  DeckManager.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/11.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "DeckManager.h"
#include "cocostudio/CocoStudio.h"
#include "DataManager.h"
#include "UserDefaultsDataManager.h"
#include "DeckData.h"
#include "CardSimpleData.h"

using namespace std;
using namespace cocostudio;

static inline string getDeckKey(int idx)
{ return cocos2d::StringUtils::format("deck_%d", idx); }
static const string defaultDeckKey("default_deck");
static inline string getSortTypeKey(DeckManager::FeatureType type)
{ return cocos2d::StringUtils::format("sort_type_%d", static_cast<int>(type)); }

static const set<DeckManager::FeatureType> featureTypes = {
    DeckManager::FeatureType::Develop,
    DeckManager::FeatureType::Deck
};

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

// TODO: remove the test method
CardSimpleData* createFakeData(int card, int level)
{
    rapidjson::Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    document.AddMember("id", card, allocator);
    document.AddMember("level", level, allocator);
    document.AddMember("amount", 50, allocator);
    return new (nothrow) CardSimpleData(document);
}

DeckManager::DeckManager()
:_defaultId(0)
,_defaultDeckData(nullptr)
{
    const int idx = UserDefaultsDataManager::getIntegerForKey(defaultDeckKey.c_str(), 0);
    _defaultId = MAX(MIN(DecksMaxCount - 1, idx), 0);
    
    for (auto ft : featureTypes) {
        auto st = static_cast<SortType>(UserDefaultsDataManager::getIntegerForKey(getSortTypeKey(ft).c_str(), 0));
        _sortTypes.insert(make_pair(ft, st));
    }
    
    static const string fake("21110;21111|22001;22003;22004;22012;22013;22014;22015;22016");
    
    for (int i = 0; i < DecksMaxCount; ++i) {
        const auto& string = UserDefaultsDataManager::getStringForKey(getDeckKey(i).c_str(), fake);
        if (string.size() > 0) {
            auto data = new (nothrow) DeckData(string);
            _decks.insert(make_pair(i, data));
        }
    }
    
    const auto& cards = DataManager::getInstance()->getCardDecks();
    for (auto iter = begin(cards); iter != end(cards); ++iter) {
        const int cardId(*iter);
        _allFoundCards.push_back(cardId);
        _allCards.insert(make_pair(cardId, createFakeData(cardId, 1)));
    }
    
//    _unfoundCards = {23100, 23101};
    for (auto iter = begin(_unfoundCards); iter != end(_unfoundCards); ++iter) {
        const int cardId(*iter);
        _allCards.insert(make_pair(cardId, createFakeData(cardId, 0)));
    }
    
    loadThisDeck();
}

DeckManager::~DeckManager()
{
    for (int i = 0; i < _decks.size(); ++i) {
        CC_SAFE_DELETE(_decks.at(i));
    }
    
    for (auto iter = begin(_allCards); iter != end(_allCards); ++iter) {
        CC_SAFE_DELETE(iter->second);
    }
}

#pragma mark - public
void DeckManager::parse(const rapidjson::Value& jsonDict)
{
    
}

DeckManager::SortType DeckManager::getSortType(FeatureType type) const
{
    if (_sortTypes.find(type) != end(_sortTypes)) {
        return _sortTypes.at(type);
    }
    
    return SortType::Default;
}

void DeckManager::setSortType(FeatureType ft, SortType st)
{
    bool update(true);
    if (_sortTypes.find(ft) == end(_sortTypes)) {
        _sortTypes.insert(make_pair(ft, st));
    } else if (st != _sortTypes.at(ft)) {
        _sortTypes.at(ft) = st;
    } else {
        update = false;
    }
    
    if (update) {
        UserDefaultsDataManager::setIntegerForKey(getSortTypeKey(ft).c_str(), static_cast<int>(st));
        sortAllCards(ft, true);
    }
}

int DeckManager::getThisDeckId() const
{
    return _defaultId;
}

void DeckManager::loadDeck(int idx)
{
    if (_defaultId != idx) {
        _defaultId = idx;
        UserDefaultsDataManager::setIntegerForKey(defaultDeckKey.c_str(), idx);
        loadThisDeck();
    }
}

const DeckData* DeckManager::getThisDeckData() const
{
    return getDeckData(_defaultId);
}

void DeckManager::saveThisDeckData()
{
    saveDeckData(_defaultId);
}

size_t DeckManager::getAllCardsCount() const
{
    return _allCards.size();
}

size_t DeckManager::getAllFoundCardsCount() const
{
    return _allFoundCards.size();
}

const CardSimpleData* DeckManager::getCardData(int card) const
{
    if (_allCards.find(card) != end(_allCards)) {
        return _allCards.at(card);
    }
    
    return nullptr;
}

bool DeckManager::isFound(int card) const
{
    auto iter(_allCards.find(card));
    if (iter != end(_allCards) && iter->second->getLevel() > 0) {
        return true;
    }
    
    return false;
}

const vector<int>& DeckManager::getFoundCards(FeatureType type) const
{
    return (FeatureType::Deck == type) ? _foundCards : _allFoundCards;
}

const vector<int>& DeckManager::getUnfoundCards() const
{
    return _unfoundCards;
}

void DeckManager::useCard(int used, int replaced, const function<void(int)>& callback)
{
    auto& foundCards(_foundCards);
    for (auto iter = begin(foundCards); iter != end(foundCards); ++iter) {
        if (used == (*iter)) {
            foundCards.erase(iter);
            break;
        }
    }
    
    foundCards.push_back(replaced);
    sortAllCards(FeatureType::Deck, false);
    
    if (_defaultDeckData) {
        _defaultDeckData->use(used, replaced, callback);
    }
}

void DeckManager::exchangeCard(int from, int to, const function<void(int, int)>& callback)
{
    if (_defaultDeckData) {
        _defaultDeckData->exchange(from, to, callback);
    }
}

void DeckManager::findCard(int card)
{
    for (auto iter = begin(_unfoundCards); iter != end(_unfoundCards); ++iter) {
        if (card == (*iter)) {
            _unfoundCards.erase(iter);
            break;
        }
    }
    
    _allFoundCards.push_back(card);
    _foundCards.push_back(card);
}

#pragma mark - private
DeckData* DeckManager::getDeckData(int idx) const
{
    if (_decks.size() > idx) {
        return _decks.at(idx);
    }
    
    return nullptr;
}

void DeckManager::loadThisDeck()
{
    _defaultDeckData = getDeckData(_defaultId);
    
    _foundCards.clear();
    
    unordered_set<int> container;
    for (auto card : _allFoundCards) {
        container.insert(card);
    }
    
    for (auto card : _defaultDeckData->getCards()) {
        container.erase(card);
    }
    
    for (auto card : container) {
        _foundCards.push_back(card);
    }
    
    sortAllCards(FeatureType::Deck, false);
}

void DeckManager::saveDeckData(int idx)
{
    if (_decks.size() > idx) {
        auto data = _decks.at(idx);
        string output;
        data->serialize(output);
        UserDefaultsDataManager::setStringForKey(getDeckKey(idx).c_str(), output);
    }
}

void DeckManager::sortCards(SortType type, vector<int>& cards) const
{
    if (cards.empty()) {
        return;
    }
    
    static const auto defaultSort = [](int c1, int c2) {
        return c1 < c2;
    };
    static const auto raritySort = [](int c1, int c2) {
        return defaultSort(c1, c2);
    };
    static const auto elixirSort = [this](int c1, int c2) {
        auto d1 = getCardData(c1);
        auto d2 = getCardData(c2);
        auto cost1 = d1 ? d1->getCost() : 0;
        auto cost2 = d2 ? d2->getCost() : 0;
        if (cost1 == cost2) {
            return defaultSort(c1, c2);
        }
        
        return cost1 < cost2;
    };
    static const auto dungeonSort = [](int c1, int c2) {
        return c1 < c2;
    };
    
    switch (type) {
        case SortType::Default:
            sort(cards.begin(), cards.end(), defaultSort);
            break;
        case SortType::Rarity:
            sort(cards.begin(), cards.end(), raritySort);
            break;
        case SortType::Elixir:
            sort(cards.begin(), cards.end(), elixirSort);
            break;
        case SortType::Dungeon:
            sort(cards.begin(), cards.end(), dungeonSort);
            break;
            
        default:
            break;
    }
}

void DeckManager::sortAllCards(FeatureType type, bool sortUnfound)
{
    auto st(getSortType(type));
    sortCards(st, (FeatureType::Deck == type) ? _foundCards : _allFoundCards);
    if (sortUnfound) {
        sortCards(st, _unfoundCards);
    }
}
