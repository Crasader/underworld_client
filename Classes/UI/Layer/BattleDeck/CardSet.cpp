//
//  CardSet.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardSet.h"
#include "DeckCard.h"

CardSet::~CardSet()
{
    clear();
}

void CardSet::insertCard(int cardId, DeckCard* card)
{
    if (_cards.find(cardId) == end(_cards)) {
        _cards.insert(std::make_pair(cardId, card));
    } else { CC_ASSERT(false); }
}

void CardSet::removeCard(int cardId, bool cleanup)
{
    if (_cards.find(cardId) != end(_cards)) {
        auto node(_cards.at(cardId));
        if (cleanup) {
            node->removeFromParent();
        }
        
        _cards.erase(cardId);
    }
}

void CardSet::pushPosition(const Point& point)
{
    _positions.push_back(point);
}

void CardSet::clear()
{
    for (auto iter = begin(_cards); iter != end(_cards); ++iter) {
        iter->second->removeFromParent();
    }
    
    _cards.clear();
    _positions.clear();
}

size_t CardSet::getCardsCount() const
{
    return _cards.size();
}

DeckCard* CardSet::getCard(int cardId) const
{
    if (_cards.find(cardId) != end(_cards)) {
        return _cards.at(cardId);
    }
    
    return nullptr;
}

size_t CardSet::getPositionsCount() const
{
    return _positions.size();
}

const Point& CardSet::getPosition(size_t idx) const
{
    if (_positions.size() > idx) {
        return _positions.at(idx);
    }
    
    CC_ASSERT(false);
    return Point::ZERO;
}
