//
//  CardSet.h
//  Underworld_Client
//
//  Created by Andy on 16/8/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardSet_h
#define CardSet_h

#include "cocos2d.h"

USING_NS_CC;

class BaseCard;

class CardSet
{
public:
    virtual ~CardSet();
    
    void insertCard(int cardId, BaseCard* card);
    void removeCard(int cardId, bool cleanup);
    void pushPosition(const Point& point);
    void clear();
    
    size_t getCardsCount() const;
    BaseCard* getCard(int cardId) const;
    
    size_t getPositionsCount() const;
    const Point& getPosition(size_t idx) const;
    
private:
    std::unordered_map<int, BaseCard*> _cards;
    std::vector<Point> _positions;
};

#endif /* CardSet_h */
