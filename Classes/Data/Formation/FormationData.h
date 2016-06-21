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
protected:
    typedef std::string FormationUnitType;
    
public:
    FormationData(const std::string& serializedString);
    virtual ~FormationData();
    
    const std::map<Point, FormationUnitType>& getHeroes() const;
    const std::vector<FormationUnitType>& getSpells() const;
    
    void insertHero(const Point& point, const FormationUnitType& name);
    void removeHero(const Point& point);
    void exchangeHero(const Point& p1, const Point& p2);
    
    void insertSpell(const FormationUnitType& name);
    void removeSpell(const FormationUnitType& name);
    void removeSpell(size_t idx);
    void exchangeSpell(size_t idx1, size_t idx2);
    
    void serialize(std::string& output);
    
private:
    std::map<Point, FormationUnitType> _heroes;
    std::vector<FormationUnitType> _spells;
};

#endif /* FormationData_h */
