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

class DeckData;

class DeckManager
{
public:
    static constexpr int MaxCount = 3;
    
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
    
private:
    DeckManager();
    virtual ~DeckManager();
    DeckManager(const DeckManager &) = delete; 
    DeckManager &operator =(const DeckManager &) = delete;
    
private:
    int _defaultId;
    std::unordered_map<int, DeckData*> _decks;
};

#endif /* DeckManager_h */
