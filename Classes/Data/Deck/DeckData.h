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
        None, Hero, Soldier
    };
    
public:
    DeckData(const std::string& serializedString);
    virtual ~DeckData();
    
    const std::vector<int>& getCards(Type type) const;
    void use(int used, int replaced);
    void exchange(int from, int to);
    
    void serialize(std::string& output);
    
protected:
    struct CardInfo;
    void parse(Type type, const std::string& input);
    CardInfo getInfo(int card) const;
    bool use(Type type, int used, int replaced);
    void remove(Type type, int card);
    
private:
    std::map<Type, std::vector<int>> _cards;
};

#endif /* CardDeckData_h */
