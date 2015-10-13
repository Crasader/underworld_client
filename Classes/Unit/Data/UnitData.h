//
//  UnitData.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#ifndef UnitData_h
#define UnitData_h

#include "TowerData.h"

class UnitData : public TowerData
{
public:
    UnitData(tinyxml2::XMLElement *xmlElement);
    virtual ~UnitData();
    
    int movement_speed() const;
    float corpse_decay_time() const;
    
protected:
    int _movement_speed;
    float _corpse_decay_time;
};

#endif /* UnitData_h */
