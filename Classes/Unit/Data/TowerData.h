//
//  TowerData.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#ifndef TowerData_h
#define TowerData_h

#include "Global.h"

namespace tinyxml2 { class XMLElement; }

class TowerData
{
public:
    TowerData(tinyxml2::XMLElement *xmlElement);
    virtual ~TowerData();
    
    UType type() const;
    int id() const;
    const std::string& name() const;
    int hp() const;
    int damage_min() const;
    int damage_max() const;
    float attack_speed() const;
    int attack_range() const;
    AttackType attack_type() const;
    int sight() const;
    int collision_size() const;
    
protected:
    UType _type;
    int _id;
    std::string _name;
    int _hp;
    int _damage_min;
    int _damage_max;
    float _attack_speed;
    int _attack_range;
    AttackType _attack_type;
    int _sight;
    int _collision_size;
};

#endif /* TowerData_h */
