//
//  FormationData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/6/20.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "FormationData.h"

using namespace std;

FormationData::FormationData()
{
    
}

FormationData::~FormationData()
{
    
}

const std::map<Point, std::string>& FormationData::getHeroes() const
{
    return _heroes;
}

const std::vector<std::string>& FormationData::getSpells() const
{
    return _spells;
}

void FormationData::insertHero(const Point& point, const std::string& name)
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
        const string name1 = _heroes.at(p1);
        _heroes.at(p1) = _heroes.at(p2);
        _heroes.at(p2) = name1;
    }
}

void FormationData::insertSpell(size_t idx, const std::string& name)
{
    
}

void FormationData::removeSpell(const std::string& name)
{
    
}

void FormationData::removeSpell(size_t idx)
{
    
}

void FormationData::exchangeSpell(size_t idx1, size_t idx2)
{
    
}
