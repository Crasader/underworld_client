//
//  DeckData.h
//  Underworld_Client
//
//  Created by Andy on 16/8/10.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardDeckData_h
#define CardDeckData_h

#include <string>
#include <vector>
#include <map>

class DeckData
{
public:
    enum class CardType {
        Hero, Soldier
    };
    
protected:
    typedef std::string Card;
    
public:
    DeckData(const std::string& serializedString);
    DeckData(const DeckData* instance);
    virtual ~DeckData();
    
    void clone(const DeckData* instance);
    
    const std::vector<Card>& getCards(CardType type);
    void insert(CardType type, ssize_t idx, const Card& card);
    void remove(CardType type, ssize_t idx);
    void exchange(CardType type, ssize_t idx1, ssize_t idx2);
    
    void serialize(std::string& output);
    
protected:
    std::vector<Card>& getMutableCards(CardType type);
    void parse(CardType type, const std::string& input);
    
private:
    std::map<CardType, std::vector<Card>> _cards;
};

#endif /* CardDeckData_h */
