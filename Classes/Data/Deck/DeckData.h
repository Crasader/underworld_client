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
#include <functional>

class DeckData
{
public:
    static constexpr int HeroCount = 2;
    static constexpr int SoldierCount = 8;
    DeckData(const std::string& serializedString);
    virtual ~DeckData();
    
    const std::vector<int>& getCards() const;
    void use(int used, int replaced, const std::function<void(int)>& callback);
    void exchange(int from, int to, const std::function<void(int, int)>& callback);
    
    void serialize(std::string& output);
    
protected:
    void parse(const std::string& input);
    void remove(int card);
    
private:
    std::vector<int> _cards;
};

#endif /* CardDeckData_h */
