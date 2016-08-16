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
#include <set>

class DeckData;
class CardSimpleData;

class DeckManager
{
public:
    static constexpr int DecksMaxCount = 3;
    
    static DeckManager* getInstance();
    static void purge();
    
    void parse(const rapidjson::Value& jsonDict);
    
    int getDefaultDeckId() const;
    void setDefaultDeckId(int value);
    
    DeckData* getDeckData(int idx) const;
    void setDeckData(int idx, const DeckData* data);
    void saveDeckData(int idx);
    
    DeckData* getDefaultDeckData() const;
    void saveDefaultDeckData();
    
    const std::unordered_map<int, CardSimpleData*>& getFoundCards() const;
    const std::set<int>& getUnfoundCards() const;
    
    void useCard(int used, int replaced);
    void findCard(int card);
    
private:
    DeckManager();
    virtual ~DeckManager();
    DeckManager(const DeckManager &) = delete; 
    DeckManager &operator =(const DeckManager &) = delete;
    
private:
    int _defaultId;
    std::unordered_map<int, DeckData*> _decks;
    std::unordered_map<int, CardSimpleData*> _foundCards;
    std::set<int> _unfoundCards;
};

#endif /* DeckManager_h */
