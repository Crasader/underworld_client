//
//  FormationData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/6/20.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "FormationData.h"
#include "Utils.h"

using namespace std;

static const string TypeSeparator("|");
static const string ElementSeparator(";");
static const string ParameterSeparator(",");

FormationData::FormationData(const string& serializedString)
{
    if (serializedString.size() > 0) {
        vector<string> formations;
        Utils::split(formations, serializedString, TypeSeparator);
        if (2 <= formations.size()) {
            vector<string> outputs;
            
            // heroes
            parseHeroes(formations.at(0));
            
            // spells
            parseSpells(formations.at(1));
        }
    }
}

FormationData::~FormationData()
{
    
}

const map<Coordinate32, FormationData::FormationUnitType>& FormationData::getHeroes() const
{
    return _heroes;
}

const vector<FormationData::FormationUnitType>& FormationData::getSpells() const
{
    return _spells;
}

void FormationData::insertHero(const Coordinate32& coordinate, const FormationUnitType& name)
{
    if (_heroes.find(coordinate) != end(_heroes)) {
        assert(false);
    } else {
        _heroes.insert(make_pair(coordinate, name));
    }
}

void FormationData::removeHero(const Coordinate32& coordinate)
{
    if (_heroes.find(coordinate) != end(_heroes)) {
        _heroes.erase(coordinate);
    }
}

void FormationData::exchangeHero(const Coordinate32& c1, const Coordinate32& c2)
{
    if (_heroes.find(c1) != end(_heroes) &&
        _heroes.find(c2) != end(_heroes)) {
        const auto name1 = _heroes.at(c1);
        _heroes.at(c1) = _heroes.at(c2);
        _heroes.at(c2) = name1;
    }
}

void FormationData::insertSpell(const FormationUnitType& name)
{
    _spells.push_back(name);
}

void FormationData::removeSpell(const FormationUnitType& name)
{
    for (auto iter = begin(_spells); iter != end(_spells); ++iter) {
        if (name == *iter) {
            _spells.erase(iter);
            break;
        }
    }
}

void FormationData::removeSpell(size_t idx)
{
    int i = 0;
    for (auto iter = begin(_spells); iter != end(_spells); ++iter, ++i) {
        if (i == idx) {
            _spells.erase(iter);
            break;
        }
    }
}

void FormationData::exchangeSpell(size_t idx1, size_t idx2)
{
    const auto cnt = _spells.size();
    if (idx1 < cnt && idx2 < cnt) {
        const auto name1 = _spells.at(idx1);
        _spells.at(idx1) = _spells.at(idx2);
        _spells.at(idx2) = name1;
    }
}

void FormationData::serialize(string& output)
{
    output.clear();
    
    // serialized string like this:
    // x1,y1,hero1;x2,y2,hero2;...xN,yN,heroN|spell1,spell2,...spellN
    for (auto iter = begin(_heroes); iter != end(_heroes); ++iter) {
        const auto& coordinate = iter->first;
        output += Utils::format("%d", coordinate.x) + ParameterSeparator;
        output += Utils::format("%d", coordinate.y) + ParameterSeparator;
        output += iter->second + ElementSeparator;
    }
    
    output = output.substr(0, output.size() - 1);
    
    output += TypeSeparator;
    
    for (int i = 0; i < _spells.size(); ++i) {
        output += _spells.at(i) + ElementSeparator;
    }
    
    if (TypeSeparator != &output.back() || 1 == output.size()) {
        output = output.substr(0, output.size() - 1);
    }
}

void FormationData::parseHeroes(const string& input)
{
    if (input.size() > 0) {
        vector<string> outputs;
        Utils::split(outputs, input, ElementSeparator);
        for (int i = 0; i < outputs.size(); ++i) {
            const auto& individual = outputs.at(i);
            vector<string> splits;
            Utils::split(splits, individual, ParameterSeparator);
            if (3 == splits.size()) {
                Coordinate32 point;
                point.x = atoi(splits.at(0).c_str());
                point.y = atoi(splits.at(1).c_str());
                _heroes.insert(make_pair(point, splits.at(2)));
            }
        }
    }
}

void FormationData::parseSpells(const string& input)
{
    if (input.size() > 0) {
        vector<string> outputs;
        Utils::split(outputs, input, ElementSeparator);
        for (int i = 0; i < outputs.size(); ++i) {
            _spells.push_back(outputs.at(i));
        }
    }
}
