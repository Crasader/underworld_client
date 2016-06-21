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

FormationData::FormationData(const string& serializedString)
{
    if (serializedString.size() > 0) {
        vector<string> formations;
        Utils::split(formations, serializedString, "|");
        if (2 <= formations.size()) {
            vector<string> outputs;
            
            // heroes
            Utils::split(outputs, formations.at(0), ";");
            for (int i = 0; i < outputs.size(); ++i) {
                const auto& individual = outputs.at(i);
                vector<string> splits;
                Utils::split(splits, individual, ",");
                if (3 == splits.size()) {
                    Point point;
                    point.x = atoi(splits.at(0).c_str());
                    point.y = atoi(splits.at(1).c_str());
                    _heroes.insert(make_pair(point, splits.at(2)));
                }
            }
            
            // spells
            outputs.clear();
            Utils::split(outputs, formations.at(1), ";");
            for (int i = 0; i < outputs.size(); ++i) {
                _spells.push_back(outputs.at(i));
            }
        }
    }
}

FormationData::~FormationData()
{
    
}

const map<Point, FormationData::FormationUnitType>& FormationData::getHeroes() const
{
    return _heroes;
}

const vector<FormationData::FormationUnitType>& FormationData::getSpells() const
{
    return _spells;
}

void FormationData::insertHero(const Point& point, const FormationUnitType& name)
{
    if (_heroes.find(point) != end(_heroes)) {
        assert(false);
    } else {
        _heroes.insert(make_pair(point, name));
    }
}

void FormationData::removeHero(const Point& point)
{
    if (_heroes.find(point) != end(_heroes)) {
        _heroes.erase(point);
    }
}

void FormationData::exchangeHero(const Point& p1, const Point& p2)
{
    if (_heroes.find(p1) != end(_heroes) &&
        _heroes.find(p2) != end(_heroes)) {
        const auto name1 = _heroes.at(p1);
        _heroes.at(p1) = _heroes.at(p2);
        _heroes.at(p2) = name1;
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
        const auto& point = iter->first;
        output += StringUtils::format("%d,", (int)point.x);
        output += StringUtils::format("%d,", (int)point.y);
        output += iter->second + ";";
    }
    
    output = output.substr(0, output.size() - 1);
    
    if (output.size() > 0) {
        output += "|";
    }
    
    for (int i = 0; i < _spells.size(); ++i) {
        output += _spells.at(i) + ";";
    }
    
    output = output.substr(0, output.size() - 1);
}
