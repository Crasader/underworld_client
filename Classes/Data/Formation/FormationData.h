//
//  FormationData.h
//  Underworld_Client
//
//  Created by Andy on 16/6/20.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef FormationData_h
#define FormationData_h

#include <map>
#include <vector>
#include "Coordinate.h"

using namespace UnderWorld::Core;

class FormationData
{
protected:
    typedef std::string FormationUnitType;
    
public:
    FormationData(const std::string& serializedString);
    virtual ~FormationData();
    
    const std::map<Coordinate32, FormationUnitType>& getHeroes() const;
    const std::vector<FormationUnitType>& getSpells() const;
    
    void insertHero(const Coordinate32& coordinate, const FormationUnitType& name);
    void removeHero(const Coordinate32& coordinate);
    void exchangeHero(const Coordinate32& c1, const Coordinate32& c2);
    
    void insertSpell(const FormationUnitType& name);
    void removeSpell(const FormationUnitType& name);
    void removeSpell(size_t idx);
    void exchangeSpell(size_t idx1, size_t idx2);
    
    void serialize(std::string& output);
    
private:
    std::map<Coordinate32, FormationUnitType> _heroes;
    std::vector<FormationUnitType> _spells;
};

#endif /* FormationData_h */
