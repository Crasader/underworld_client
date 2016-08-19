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

const string DeckManager::SortNotification = "SortNotification";

static inline string getDeckKey(int idx)
{ return cocos2d::StringUtils::format("deck_%d", idx); }
static const string defaultDeckKey("default_deck");
static const string sortTypeKey("sort_type");

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

CardSimpleData* DeckManager::createFakeData(int card)
{
    rapidjson::Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    document.AddMember("id", card, allocator);
    document.AddMember("level", 1, allocator);
    return new (nothrow) CardSimpleData(document);
}

DeckManager::DeckManager()
:_defaultId(0)
,_defaultDeckData(nullptr)
,_sortType(SortType::Default)
{
    const int idx = UserDefaultsDataManager::getIntegerForKey(defaultDeckKey.c_str(), 0);
    _defaultId = MAX(MIN(DecksMaxCount - 1, idx), 0);
    _sortType = static_cast<SortType>(UserDefaultsDataManager::getIntegerForKey(sortTypeKey.c_str(), 0));
    
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
        auto data(createFakeData(*iter));
        _allFoundCards.insert(make_pair(data->getIdx(), data));
    }
    
    loadThisDeck();
}

DeckManager::~DeckManager()
{
    for (int i = 0; i < _decks.size(); ++i) {
        CC_SAFE_DELETE(_decks.at(i));
    }
    
    for (auto iter = begin(_allFoundCards); iter != end(_allFoundCards); ++iter) {
        CC_SAFE_DELETE(iter->second);
    }
}

#pragma mark - public
void DeckManager::parse(const rapidjson::Value& jsonDict)
{
    
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

DeckManager::SortType DeckManager::getSortType() const
{
    return _sortType;
}

void DeckManager::setSortType(SortType type)
{
    if (_sortType != type) {
        _sortType = type;
        UserDefaultsDataManager::setIntegerForKey(sortTypeKey.c_str(), static_cast<int>(type));
        sortFoundCards();
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
    return getAllFoundCardsCount() + _unfoundCards.size();
}

size_t DeckManager::getAllFoundCardsCount() const
{
    return _allFoundCards.size();
}

const CardSimpleData* DeckManager::getCardData(int card) const
{
    if (_allFoundCards.find(card) != end(_allFoundCards)) {
        return _allFoundCards.at(card);
    }
    
    return nullptr;
}

const vector<int>& DeckManager::getFoundCards() const
{
    return _foundCards;
}

const vector<int>& DeckManager::getUnfoundCards() const
{
    return _unfoundCards;
}

void DeckManager::useCard(int used, int replaced, const function<void(int)>& callback)
{
    for (auto iter = begin(_foundCards); iter != end(_foundCards); ++iter) {
        if (used == (*iter)) {
            _foundCards.erase(iter);
            break;
        }
    }
    
    _foundCards.push_back(replaced);
    sortFoundCards();
    
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
    
    auto data(createFakeData(card));
    _allFoundCards.insert(make_pair(data->getIdx(), data));
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
    for (auto iter = begin(_allFoundCards); iter != end(_allFoundCards); ++iter) {
        container.insert(iter->first);
    }
    
    for (auto card : _defaultDeckData->getCards()) {
        container.erase(card);
    }
    
    for (auto card : container) {
        _foundCards.push_back(card);
    }
    
    sortFoundCards();
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

void DeckManager::sortFoundCards()
{
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
    
    switch (_sortType) {
        case SortType::Default:
            sort(_foundCards.begin(), _foundCards.end(), defaultSort);
            break;
        case SortType::Rarity:
            sort(_foundCards.begin(), _foundCards.end(), raritySort);
            break;
        case SortType::Elixir:
            sort(_foundCards.begin(), _foundCards.end(), elixirSort);
            break;
        case SortType::Dungeon:
            sort(_foundCards.begin(), _foundCards.end(), dungeonSort);
            break;
            
        default:
            break;
    }
    
    cocos2d::Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(SortNotification);
}
