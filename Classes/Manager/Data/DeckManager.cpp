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
#include "CardData.h"
#include "RuneData.h"
#include "RuneGroupData.h"
#include "NetworkApi.h"
#include "ResourceManager.h"

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
    
    static const string fake("21110;21111;21100|22001;22003;22004;22012;22013;22014");
    
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
    
//    _allUnfoundCards = {23100, 23101};
    for (auto iter = begin(_allUnfoundCards); iter != end(_allUnfoundCards); ++iter) {
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
    
    for (auto iter = begin(_cardDetails); iter != end(_cardDetails); ++iter) {
        CC_SAFE_DELETE(iter->second);
    }
    
    for (auto iter = begin(_runeGroups); iter != end(_runeGroups); ++iter) {
        CC_SAFE_DELETE(iter->second);
    }
}

#pragma mark - public
#pragma mark - network request
void DeckManager::getCardList(const function<void()>& callback)
{
    if (_allCards.empty()) {
        NetworkApi::getCardList([this, callback](long code, const rapidjson::Value& jsonDict) {
            {
                static const char* key("cards");
                if (DICTOOL->checkObjectExist_json(jsonDict, key)) {
                    for (int i = 0; i < DICTOOL->getArrayCount_json(jsonDict, key); ++i) {
                        const auto& value = DICTOOL->getDictionaryFromArray_json(jsonDict, key, i);
                        auto data = new (nothrow) CardSimpleData(value);
                        auto cardId(data->getId());
                        _allCards.insert(make_pair(cardId, data));
                        _allFoundCards.push_back(cardId);
                    }
                }
            }
            
            {
                static const char* key("ncards");
                if (DICTOOL->checkObjectExist_json(jsonDict, key)) {
                    for (int i = 0; i < DICTOOL->getArrayCount_json(jsonDict, key); ++i) {
                        const auto& value = DICTOOL->getDictionaryFromArray_json(jsonDict, key, i);
                        auto data = new (nothrow) CardSimpleData(value);
                        auto cardId(data->getId());
                        _allCards.insert(make_pair(cardId, data));
                        _allUnfoundCards.push_back(cardId);
                    }
                }
            }
            
            if (callback) {
                callback();
            }
        });
    } else if (callback) {
        callback();
    }
}

void DeckManager::getCardDetail(int cardId, const function<void(const CardData*)>& callback)
{
    auto data(getCardData(cardId));
    if (data) {
        auto detail(getCardDetail(cardId));
        if (!detail) {
            // the card not found
            if (data->getLevel() > 1 || data->getAmount() > 0) {
                NetworkApi::getCardDetail(data->getDbId(), [this, callback](long code, const rapidjson::Value& jsonDict) {
                    auto data = updateCardData(jsonDict);
                    if (callback) {
                        callback(data);
                    }
                });
            } else {
                
            }
        } else if (callback) {
            callback(detail);
        }
    }
}

void DeckManager::upgradeCard(int cardId, const function<void(const CardData*)>& callback)
{
    auto data(getCardData(cardId));
    if (data) {
        NetworkApi::upgradeCard(data->getDbId(), data->getLevel(), [this, callback](long code, const rapidjson::Value& jsonDict) {
            ResourceManager::getInstance()->updateResources(jsonDict);
            auto data = updateCardData(jsonDict);
            if (callback) {
                callback(data);
            }
        });
    }
}

void DeckManager::upgradeCardSkill(int cardId, int skillIdx, const function<void(const CardData*)>& callback)
{
    auto data(getCardDetail(cardId));
    if (data && data->getSkills().size() > skillIdx) {
        NetworkApi::upgradeCardSkill(data->getDbId(), data->getLevel(), skillIdx, [this, callback](long code, const rapidjson::Value& jsonDict) {
            ResourceManager::getInstance()->updateResources(jsonDict);
            auto data = updateCardData(jsonDict);
            if (callback) {
                callback(data);
            }
        });
    }
}

void DeckManager::getRunesList(const std::function<void()>& callback)
{
    if (_runeGroups.empty()) {
        NetworkApi::getRunesList([=](long code, const rapidjson::Value& jsonDict) {
            static const char* key("runepacks");
            if (DICTOOL->checkObjectExist_json(jsonDict, key)) {
                for (int i = 0; i < DICTOOL->getArrayCount_json(jsonDict, key); ++i) {
                    const auto& value = DICTOOL->getDictionaryFromArray_json(jsonDict, key, i);
                    auto data = new (nothrow) RuneGroupData(value);
                    auto dbId(data->getDbId());
                    _runeGroups.insert(make_pair(dbId, data));
                }
            }
            
            if (callback) {
                callback();
            }
        });
    } else if (callback) {
        callback();
    }
}

void DeckManager::imbedRune(int cardId, int runeIdx, int dbId, const function<void()>& callback)
{
    auto data(getCardDetail(cardId));
    if (data) {
        CC_ASSERT(_runeGroups.find(dbId) != end(_runeGroups));
        NetworkApi::imbedRune(data->getDbId(), runeIdx, dbId, [=](long code, const rapidjson::Value& jsonDict) {
            updateRune(cardId, runeIdx, jsonDict);
            updateRuneGroups(jsonDict);
            
            if (callback) {
                callback();
            }
        });
    }
}

void DeckManager::unloadRune(int cardId, int runeIdx, const function<void()>& callback)
{
    auto data(getCardDetail(cardId));
    if (data) {
        NetworkApi::unloadRune(data->getDbId(), runeIdx, [=](long code, const rapidjson::Value& jsonDict) {
            static const char* key("runepack");
            if (DICTOOL->checkObjectExist_json(jsonDict, key)) {
                const auto& value = DICTOOL->getSubDictionary_json(jsonDict, key);
                updateRuneGroup(value);
            }
            
            removeRune(cardId, runeIdx);
            
            if (callback) {
                callback();
            }
        });
    }
}

void DeckManager::upgradeRune(int cardId, int runeIdx, const function<void()>& callback)
{
    auto data(getCardDetail(cardId));
    if (data) {
        auto rune(data->getRune(runeIdx));
        if (rune) {
            NetworkApi::upgradeRune(rune->getDbId(), rune->getLevel(), [=](long code, const rapidjson::Value& jsonDict) {
                ResourceManager::getInstance()->updateResources(jsonDict);
                updateRune(cardId, runeIdx, jsonDict);
                static const char* key("runepack");
                if (DICTOOL->checkObjectExist_json(jsonDict, key)) {
                    const auto& value = DICTOOL->getSubDictionary_json(jsonDict, key);
                    updateRuneGroup(value);
                }
                
                if (callback) {
                    callback();
                }
            });
        }
    }
}

void DeckManager::compoundRune(int dbId, const function<void()>& callback)
{
    CC_ASSERT(_runeGroups.find(dbId) != end(_runeGroups));
    NetworkApi::compoundRune(dbId, [=](long code, const rapidjson::Value& jsonDict) {
        ResourceManager::getInstance()->updateResources(jsonDict);
        updateRuneGroups(jsonDict);
        
        if (callback) {
            callback();
        }
    });
}

#pragma mark - sort
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

#pragma mark - deck
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

#pragma mark - card
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

const CardData* DeckManager::getCardDetail(int card) const
{
    if (_cardDetails.find(card) != end(_cardDetails)) {
        return _cardDetails.at(card);
    }
    
    return nullptr;
}

bool DeckManager::isFound(int card) const
{
    auto data(getCardData(card));
    if (data && data->getLevel() > 0) {
        return true;
    }
    
    return false;
}

const vector<int>& DeckManager::getFoundCards(FeatureType type) const
{
    return (FeatureType::Deck == type) ? _offDeckFoundCards : _allFoundCards;
}

const vector<int>& DeckManager::getUnfoundCards() const
{
    return _allUnfoundCards;
}

#pragma mark - rune
const unordered_map<int, RuneGroupData*>& DeckManager::getRuneGroups() const
{
    return _runeGroups;
}

#pragma mark - offline functions
void DeckManager::useCard(int used, int replaced)
{
    auto& foundCards(_offDeckFoundCards);
    for (auto iter = begin(foundCards); iter != end(foundCards); ++iter) {
        if (used == (*iter)) {
            foundCards.erase(iter);
            break;
        }
    }
    
    foundCards.push_back(replaced);
    sortAllCards(FeatureType::Deck, false);
    
    if (_defaultDeckData) {
        _defaultDeckData->use(used, replaced);
    }
}

void DeckManager::exchangeCard(int from, int to)
{
    if (_defaultDeckData) {
        _defaultDeckData->exchange(from, to);
    }
}

void DeckManager::findCard(int card)
{
    for (auto iter = begin(_allUnfoundCards); iter != end(_allUnfoundCards); ++iter) {
        if (card == (*iter)) {
            _allUnfoundCards.erase(iter);
            break;
        }
    }
    
    _allFoundCards.push_back(card);
    _offDeckFoundCards.push_back(card);
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
    
    _offDeckFoundCards.clear();
    
    unordered_set<int> container;
    for (auto card : _allFoundCards) {
        container.insert(card);
    }
    
    for (auto card : _defaultDeckData->getCards()) {
        container.erase(card);
    }
    
    for (auto card : container) {
        _offDeckFoundCards.push_back(card);
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
    sortCards(st, (FeatureType::Deck == type) ? _offDeckFoundCards : _allFoundCards);
    if (sortUnfound) {
        sortCards(st, _allUnfoundCards);
    }
}

const CardData* DeckManager::updateCardData(const rapidjson::Value& jsonDict)
{
    const auto& value = DICTOOL->getSubDictionary_json(jsonDict, "card");
    auto cardId(DICTOOL->getIntValue_json(value, "id"));
    if (_cardDetails.find(cardId) != end(_cardDetails)) {
        _cardDetails.at(cardId)->update(value);
    } else {
        _cardDetails.insert(make_pair(cardId, new (nothrow) CardData(value)));
    }
    
    return _cardDetails.at(cardId);
}

void DeckManager::removeRune(int cardId, int runeIdx)
{
    if (_cardDetails.find(cardId) != end(_cardDetails)) {
        _cardDetails.at(cardId)->removeRune(runeIdx);
    }
}

void DeckManager::updateRune(int cardId, int runeIdx, const rapidjson::Value& jsonDict)
{
    static const char* key("rune");
    if (_cardDetails.find(cardId) != end(_cardDetails) && DICTOOL->checkObjectExist_json(jsonDict, key)) {
        auto data(_cardDetails.at(cardId));
        if (data) {
            const auto& value = DICTOOL->getSubDictionary_json(jsonDict, key);
            data->updateRune(runeIdx, value);
        }
    }
}

void DeckManager::updateRuneGroup(const rapidjson::Value& jsonDict)
{
    static const char* key("db");
    if (DICTOOL->checkObjectExist_json(jsonDict, key)) {
        auto dbId(DICTOOL->getIntValue_json(jsonDict, key));
        if (_runeGroups.find(dbId) != end(_runeGroups)) {
            auto data(_runeGroups.at(dbId));
            data->update(jsonDict);
            if (data->getAmount() <= 0) {
                _runeGroups.erase(dbId);
            }
        } else {
            _runeGroups.insert(make_pair(dbId, new (nothrow) RuneGroupData(jsonDict)));
        }
    }
}

void DeckManager::updateRuneGroups(const rapidjson::Value& jsonDict)
{
    static const char* key("runepacks");
    if (DICTOOL->checkObjectExist_json(jsonDict, key)) {
        for (int i = 0; i < DICTOOL->getArrayCount_json(jsonDict, key); ++i) {
            const auto& value = DICTOOL->getDictionaryFromArray_json(jsonDict, key, i);
            updateRuneGroup(value);
        }
    }
}
