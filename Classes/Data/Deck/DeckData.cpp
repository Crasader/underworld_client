//
//  DeckData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/10.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "DeckData.h"
#include "Utils.h"

using namespace std;

static const string TypeSeparator("|");
static const string ElementSeparator(";");

DeckData::DeckData(const string& serializedString)
{
    if (serializedString.size() > 0) {
        vector<string> msgs;
        Utils::split(msgs, serializedString, TypeSeparator);
        if (2 <= msgs.size()) {
            parse(Type::Hero, msgs.at(0));
            parse(Type::Soldier, msgs.at(1));
        }
    }
}

DeckData::DeckData(const DeckData* instance)
{
    clone(instance);
}

DeckData::~DeckData() {}

void DeckData::clone(const DeckData* instance)
{
    if (instance) {
        const auto& cardsMap(instance->_cards);
        for (auto iter = begin(cardsMap); iter != end(cardsMap); ++iter) {
            const auto& type(iter->first);
            auto& cards(getMutableCards(type));
            const auto& value(iter->second);
            for (int i = 0; i < value.size(); ++i) {
                cards.push_back(value.at(i));
            }
        }
    }
}

const vector<int>& DeckData::getCards(Type type)
{
    return getMutableCards(type);
}

void DeckData::insert(Type type, ssize_t idx, int card)
{
    auto& cards(getMutableCards(type));
    if (idx < cards.size()) {
        cards.at(idx) = card;
    }
}

void DeckData::exchange(Type type, ssize_t idx1, ssize_t idx2)
{
    auto& cards(getMutableCards(type));
    const auto cnt = cards.size();
    if (idx1 < cnt && idx2 < cnt) {
        const auto name1 = cards.at(idx1);
        cards.at(idx1) = cards.at(idx2);
        cards.at(idx2) = name1;
    }
}

void DeckData::serialize(string& output)
{
    output.clear();
    
    auto& heroes(getMutableCards(Type::Hero));
    // serialized string like this:
    // hero1;hero2;...heroN|spell1;spell2;...spellN
    for (int i = 0; i < heroes.size(); ++i) {
        output += to_string(heroes.at(i)) + ElementSeparator;
    }
    
    output = output.substr(0, output.size() - 1);
    
    output += TypeSeparator;
    
    auto& soldiers(getMutableCards(Type::Soldier));
    for (int i = 0; i < soldiers.size(); ++i) {
        output += to_string(soldiers.at(i)) + ElementSeparator;
    }
    
    if (TypeSeparator != &output.back() || 1 == output.size()) {
        output = output.substr(0, output.size() - 1);
    }
}

vector<int>& DeckData::getMutableCards(Type type)
{
    if (_cards.find(type) == end(_cards)) {
        _cards.insert(make_pair(type, vector<int>()));
    }
    
    return _cards.at(type);
}

void DeckData::parse(Type type, const string& input)
{
    if (!input.empty()) {
        vector<string> outputs;
        Utils::split(outputs, input, ElementSeparator);
        auto& cards(getMutableCards(type));
        for (int i = 0; i < outputs.size(); ++i) {
            cards.push_back(atoi(outputs.at(i).c_str()));
        }
    }
}

void DeckData::remove(Type type, ssize_t idx)
{
    int i = 0;
    auto& cards(getMutableCards(type));
    for (auto iter = begin(cards); iter != end(cards); ++iter, ++i) {
        if (i == idx) {
            cards.erase(iter);
            break;
        }
    }
}
