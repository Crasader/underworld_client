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

struct DeckData::CardInfo {
    Type type;
    int idx;
};

DeckData::DeckData(const string& serializedString)
{
    _cards.insert(make_pair(Type::Hero, vector<int>()));
    _cards.insert(make_pair(Type::Soldier, vector<int>()));
    
    if (serializedString.size() > 0) {
        vector<string> msgs;
        Utils::split(msgs, serializedString, TypeSeparator);
        if (2 <= msgs.size()) {
            parse(Type::Hero, msgs.at(0));
            parse(Type::Soldier, msgs.at(1));
        }
    }
}

DeckData::~DeckData() {}

const vector<int>& DeckData::getCards(Type type) const
{
    return _cards.at(type);
}

void DeckData::use(int used, int replaced)
{
    if (!use(Type::Hero, used, replaced)) {
        use(Type::Soldier, used, replaced);
    }
}

void DeckData::exchange(int from, int to)
{
    CardInfo c1 = getInfo(from);
    CardInfo c2 = getInfo(to);
    
    if (c1.type == c2.type) {
        if (Type::None != c1.type) {
            auto& cards(_cards.at(c1.type));
            cards.at(c1.idx) = to;
            cards.at(c2.idx) = from;
        } else { assert(false); }
    } else {
        if (Type::None != c1.type) {
            _cards.at(c1.type).at(c1.idx) = to;
        } else { assert(false); }
        
        if (Type::None != c2.type) {
            _cards.at(c2.type).at(c2.idx) = from;
        } else { assert(false); }
    }
}

void DeckData::serialize(string& output)
{
    output.clear();
    
    auto& heroes(_cards.at(Type::Hero));
    // serialized string like this:
    // hero1;hero2;...heroN|spell1;spell2;...spellN
    for (int i = 0; i < heroes.size(); ++i) {
        output += UnderWorld::Core::UnderWorldCoreUtils::to_string(heroes.at(i)) + ElementSeparator;
    }
    
    output = output.substr(0, output.size() - 1);
    
    output += TypeSeparator;
    
    auto& soldiers(_cards.at(Type::Soldier));
    for (int i = 0; i < soldiers.size(); ++i) {
        output += UnderWorld::Core::UnderWorldCoreUtils::to_string(soldiers.at(i)) + ElementSeparator;
    }
    
    if (TypeSeparator != &output.back() || 1 == output.size()) {
        output = output.substr(0, output.size() - 1);
    }
}

void DeckData::parse(Type type, const string& input)
{
    if (!input.empty()) {
        vector<string> outputs;
        Utils::split(outputs, input, ElementSeparator);
        auto& cards(_cards.at(type));
        for (int i = 0; i < outputs.size(); ++i) {
            cards.push_back(atoi(outputs.at(i).c_str()));
        }
    }
}

DeckData::CardInfo DeckData::getInfo(int card) const
{
    for (auto iter = begin(_cards); iter != end(_cards); ++iter) {
        const auto& cv(iter->second);
        for (int i = 0; i < cv.size(); ++i) {
            if (cv.at(i) == card) {
                return {iter->first, i};
            }
        }
    }
    
    return {Type::None, -1};
}

bool DeckData::use(Type type, int used, int replaced)
{
    auto& cards(_cards.at(type));
    for (int i = 0; i < cards.size(); ++i) {
        if (replaced == cards.at(i)) {
            cards.at(i) = used;
            return true;
        }
    }
    
    return false;
}

void DeckData::remove(Type type, int card)
{
    int i = 0;
    auto& cards(_cards.at(type));
    for (auto iter = begin(cards); iter != end(cards); ++iter, ++i) {
        if (card == (*iter)) {
            cards.erase(iter);
            break;
        }
    }
}
