//
//  DeckData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/10.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "DeckData.h"
#include "Utils.h"
#include "CoreUtils.h"

using namespace std;

static const string TypeSeparator("|");
static const string ElementSeparator(";");

DeckData::DeckData(const string& serializedString)
{
    if (serializedString.size() > 0) {
        vector<string> msgs;
        Utils::split(msgs, serializedString, TypeSeparator);
        static const size_t typesCount(2);
        const auto cnt(msgs.size());
        assert(cnt == typesCount);
        for (int i = 0; i < (cnt > typesCount ? typesCount : cnt); ++i) {
            parse(msgs.at(i));
        }
    }
}

DeckData::~DeckData() {}

const vector<int>& DeckData::getCards() const
{
    return _cards;
}

void DeckData::use(int used, int replaced, const function<void(int)>& callback)
{
    for (int i = 0; i < _cards.size(); ++i) {
        if (replaced == _cards.at(i)) {
            _cards.at(i) = used;
            if (callback) {
                callback(i);
            }
            break;
        }
    }
}

void DeckData::exchange(int from, int to, const function<void(int, int)>& callback)
{
    int idxFrom(-1);
    int idxTo(-1);
    for (int i = 0; i < _cards.size(); ++i) {
        if (from == _cards.at(i)) {
            idxFrom = i;
        } else if (to == _cards.at(i)) {
            idxTo = i;
        }
        
        if (idxFrom >= 0 && idxTo >= 0) {
            break;
        }
    }
    
    if (idxFrom >= 0 && idxTo >= 0) {
        _cards.at(idxFrom) = to;
        _cards.at(idxTo) = from;
        if (callback) {
            callback(idxFrom, idxTo);
        }
    }
}

void DeckData::serialize(string& output)
{
    output.clear();
    
    const auto cnt(_cards.size());
    assert(cnt == HeroCount + SoldierCount);
    
    for (int i = 0; i < cnt; ++i) {
        if (HeroCount == i) {
            output = output.substr(0, output.size() - 1) + TypeSeparator;
        }
        
        const int cardId(_cards.at(i));
        if (cardId > 0) {
            output += UnderWorld::Core::UnderWorldCoreUtils::to_string(cardId) + ElementSeparator;
        }
    }
    
    if (TypeSeparator != &output.back() || 1 == output.size()) {
        output = output.substr(0, output.size() - 1);
    }
}

void DeckData::parse(const string& input)
{
    if (!input.empty()) {
        vector<string> outputs;
        Utils::split(outputs, input, ElementSeparator);
        for (int i = 0; i < outputs.size(); ++i) {
            _cards.push_back(atoi(outputs.at(i).c_str()));
        }
    }
}

void DeckData::remove(int card)
{
    int i = 0;
    for (auto iter = begin(_cards); iter != end(_cards); ++iter, ++i) {
        if (card == (*iter)) {
            _cards.erase(iter);
            break;
        }
    }
}
