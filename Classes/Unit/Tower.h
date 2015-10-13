//
//  Tower.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#ifndef Tower_h
#define Tower_h

#include "Global.h"

class TowerData;

class Tower
{
public:
    Tower(int towerId);
    virtual ~Tower();
    
    int getId() const;
    const TowerData* getTowerData() const;
    
protected:
    int _id;
    const TowerData* _towerData;
};

#endif /* Tower_h */
