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

class DeckManager
{
public:
    static constexpr int DecksMaxCount = 3;
    
    enum class CardType {
        Hero,
        Soldier,
        Spell,
        Building,
    };
    
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
    
    static CardType getCardType(int cardId);
    static bool isHero(int cardId);
    
    void parse(const rapidjson::Value& jsonDict);
    
    SortType getSortType(FeatureType type) const;
    void setSortType(FeatureType ft, SortType st);
    
    int getThisDeckId() const;
    void loadDeck(int idx);
    
    const DeckData* getThisDeckData() const;
    void saveThisDeckData();
    
    size_t getAllCardsCount() const;
    size_t getAllFoundCardsCount() const;
    const CardSimpleData* getCardData(int card) const;
    bool isFound(int card) const;
    const std::vector<int>& getFoundCards(FeatureType type) const;
    const std::vector<int>& getUnfoundCards() const;
    
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
    
private:
    int _defaultId;
    DeckData* _defaultDeckData;
    std::unordered_map<int, DeckData*> _decks;
    std::unordered_map<int, CardSimpleData*> _allCards;
    std::map<FeatureType, SortType> _sortTypes;
    std::vector<int> _allFoundCards;
    std::vector<int> _foundCards;
    std::vector<int> _unfoundCards;
};

#endif /* DeckManager_h */
