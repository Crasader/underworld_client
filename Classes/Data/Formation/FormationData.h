//
//  FormationData.h
//  Underworld_Client
//
//  Created by Andy on 16/6/20.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef FormationData_h
#define FormationData_h

#include "cocos2d.h"
#include <map>
#include <vector>

USING_NS_CC;

class FormationData
{
public:
    FormationData();
    virtual ~FormationData();
    
    const std::map<Point, std::string>& getHeroes() const;
    const std::vector<std::string>& getSpells() const;
    
    void insertHero(const Point& point, const std::string& name);
    void removeHero(const Point& point);
    void exchangeHero(const Point& p1, const Point& p2);
    
    void insertSpell(size_t idx, const std::string& name);
    void removeSpell(const std::string& name);
    void removeSpell(size_t idx);
    void exchangeSpell(size_t idx1, size_t idx2);
    
private:
    std::map<Point, std::string> _heroes;
    std::vector<std::string> _spells;
};

#endif /* FormationData_h */
