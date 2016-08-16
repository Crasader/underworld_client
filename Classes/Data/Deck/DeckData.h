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
    enum class Type {
        Hero, Soldier
    };
    
public:
    DeckData(const std::string& serializedString);
    DeckData(const DeckData* instance);
    virtual ~DeckData();
    
    void clone(const DeckData* instance);
    
    const std::vector<int>& getCards(Type type);
    void insert(Type type, ssize_t idx, int card);
    void exchange(Type type, ssize_t idx1, ssize_t idx2);
    
    void serialize(std::string& output);
    
protected:
    std::vector<int>& getMutableCards(Type type);
    void parse(Type type, const std::string& input);
    void remove(Type type, ssize_t idx);
    
private:
    std::map<Type, std::vector<int>> _cards;
};

#endif /* CardDeckData_h */
