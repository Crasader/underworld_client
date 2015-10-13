//
//  Hero.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#ifndef Hero_h
#define Hero_h

#include "Unit.h"

class Hero : public Unit
{
public:
    Hero(int heroId);
    virtual ~Hero();
    
    int getLevel() const;
    
protected:
    int _level;
};

#endif /* Hero_h */
