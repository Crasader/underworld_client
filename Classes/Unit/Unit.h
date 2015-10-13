//
//  Unit.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#ifndef Unit_h
#define Unit_h

#include "Global.h"

class UnitData;

class Unit
{
public:
    Unit(int unitId);
    virtual ~Unit();
    
    int getId() const;
    const UnitData* getUnitData() const;
    
protected:
    int _id;
    const UnitData* _unitData;
};

#endif /* Unit_h */
