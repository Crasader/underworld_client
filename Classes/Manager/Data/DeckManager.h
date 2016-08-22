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
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <functional>

class DeckData;
class CardSimpleData;

class DeckManager
{
public:
    static constexpr int DecksMaxCount = 3;
    static const std::string SortNotification;
    
    enum class SortType {
        Default,
        Rarity,
        Elixir,
        Dungeon
    };
    
    static DeckManager* getInstance();
    static void purge();
    
    void parse(const rapidjson::Value& jsonDict);
    
    int getThisDeckId() const;
    void loadDeck(int idx);
    
    SortType getSortType() const;
    void setSortType(SortType type);
    
    const DeckData* getThisDeckData() const;
    void saveThisDeckData();
    
    size_t getAllCardsCount() const;
    size_t getAllFoundCardsCount() const;
    const CardSimpleData* getCardData(int card) const;
    bool isFound(int card) const;
    const std::vector<int>& getFoundCards() const;
    const std::vector<int>& getUnfoundCards() const;
    
    void useCard(int used, int replaced, const std::function<void(int)>& callback);
    void exchangeCard(int from, int to, const std::function<void(int, int)>& callback);
    void findCard(int card);
    
private:
    DeckManager();
    virtual ~DeckManager();
    DeckManager(const DeckManager &) = delete; 
    DeckManager &operator =(const DeckManager &) = delete;
    
    DeckData* getDeckData(int idx) const;
    void loadThisDeck();
    void saveDeckData(int idx);
    void sortFoundCards();
    
private:
    int _defaultId;
    DeckData* _defaultDeckData;
    SortType _sortType;
    std::unordered_map<int, DeckData*> _decks;
    std::unordered_map<int, CardSimpleData*> _allCards;
    std::unordered_set<int> _allFoundCards;
    std::vector<int> _foundCards;
    std::vector<int> _unfoundCards;
};

#endif /* DeckManager_h */
