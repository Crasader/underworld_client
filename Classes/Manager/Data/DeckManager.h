//
//  DeckManager.h
//  Underworld_Client
//
//  Created by Andy on 16/8/11.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef DeckManager_h
#define DeckManager_h

#include "json/document.h"
#include <unordered_map>
#include <vector>
#include <map>
#include <functional>

class DeckData;
class CardSimpleData;
class CardData;
class RuneData;
class RuneGroupData;

class DeckManager
{
public:
    static constexpr int DecksMaxCount = 3;
    
    enum class FeatureType {
        Develop,
        Deck
    };
    
    enum class SortType {
        Default,
        Rarity,
        Elixir,
        Dungeon
    };
    
    static DeckManager* getInstance();
    static void purge();
    
    // network request
    void getCardList(const std::function<void()>& callback);
    void getCardDetail(int cardId, const std::function<void(const CardData*)>& callback);
    void upgradeCard(int cardId, const std::function<void(const CardData*)>& callback);
    void upgradeCardSkill(int cardId, int skillIdx, const std::function<void(const CardData*)>& callback);
    void getRunesList(const std::function<void()>& callback);
    void imbedRune(int cardId, int runeIdx, int dbId, const std::function<void()>& callback);
    void unloadRune(int cardId, int runeIdx, const std::function<void()>& callback);
    void upgradeRune(int cardId, int runeIdx, const std::function<void()>& callback);
    void compoundRune(int dbId, const std::function<void()>& callback);
    
    // sort
    SortType getSortType(FeatureType type) const;
    void setSortType(FeatureType ft, SortType st);
    
    // deck
    int getThisDeckId() const;
    void loadDeck(int idx);
    const DeckData* getThisDeckData() const;
    void saveThisDeckData();
    
    // card
    size_t getAllCardsCount() const;
    size_t getAllFoundCardsCount() const;
    const CardSimpleData* getCardData(int card) const;
    const CardData* getCardDetail(int card) const;
    bool isFound(int card) const;
    const std::vector<int>& getFoundCards(FeatureType type) const;
    const std::vector<int>& getUnfoundCards() const;
    
    // rune
    const std::unordered_map<int, RuneGroupData*>& getRuneGroups() const;
    
    // offline functions
    void useCard(int used, int replaced);
    void exchangeCard(int from, int to);
    void findCard(int card);
    
private:
    DeckManager();
    virtual ~DeckManager();
    DeckManager(const DeckManager &) = delete; 
    DeckManager &operator =(const DeckManager &) = delete;
    
    DeckData* getDeckData(int idx) const;
    void loadThisDeck();
    void saveDeckData(int idx);
    void sortCards(SortType type, std::vector<int>& cards) const;
    void sortAllCards(FeatureType type, bool sortUnfound);
    
    const CardData* updateCardData(const rapidjson::Value& jsonDict);
    void removeRune(int cardId, int runeIdx);
    void updateRune(int cardId, int runeIdx, const rapidjson::Value& jsonDict);
    void updateRuneGroup(const rapidjson::Value& jsonDict);
    void updateRuneGroups(const rapidjson::Value& jsonDict);
    
private:
    int _defaultId;
    DeckData* _defaultDeckData;
    std::unordered_map<int, DeckData*> _decks;
    std::unordered_map<int, CardSimpleData*> _allCards;
    std::unordered_map<int, CardData*> _cardDetails;
    std::map<FeatureType, SortType> _sortTypes;
    std::vector<int> _allFoundCards;
    std::vector<int> _allUnfoundCards;
    std::vector<int> _offDeckFoundCards;
    std::unordered_map<int, RuneGroupData*> _runeGroups;
};

#endif /* DeckManager_h */
