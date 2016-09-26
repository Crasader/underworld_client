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

class DeckData
{
public:
    static constexpr int HeroCount = 3;
    static constexpr int SoldierCount = 6;
    explicit DeckData(const std::string& serializedString);
    virtual ~DeckData();
    
    const std::vector<int>& getCards() const;
    void use(int used, int replaced);
    void exchange(int from, int to);
    
    void serialize(std::string& output);
    
protected:
    void parse(const std::string& input);
    void remove(int card);
    
private:
    std::vector<int> _cards;
};

#endif /* CardDeckData_h */
