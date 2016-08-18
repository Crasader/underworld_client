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
public:
    FormationData(const std::string& serializedString);
    FormationData(const FormationData* instance);
    virtual ~FormationData();
    
    void clone(const FormationData* instance);
    
    const std::map<Coordinate32, int>& getHeroes() const;
    const std::vector<int>& getSpells() const;
    
    void insertHero(const Coordinate32& coordinate, int card);
    void removeHero(const Coordinate32& coordinate);
    void exchangeHero(const Coordinate32& c1, const Coordinate32& c2);
    
    void insertSpell(int card);
    void removeSpell(int card);
    void removeSpell(size_t idx);
    void exchangeSpell(size_t idx1, size_t idx2);
    
    void serialize(std::string& output);
    
protected:
    void parseHeroes(const std::string& input);
    void parseSpells(const std::string& input);
    
private:
    std::map<Coordinate32, int> _heroes;
    std::vector<int> _spells;
};

#endif /* FormationData_h */
