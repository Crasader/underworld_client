//
//  Global.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#ifndef Global_h
#define Global_h

#define INVALID_VALUE   (-1)

typedef enum
{
    kRaceType_Vampire,
    kRaceType_Werewolf,
}RaceType;

typedef enum
{
    kUnit,
    kHero,
    kTower,
}UType;

// Unit
enum
{
    // Vampire
    kVampireUnit_0,
    kVampireUnit_1,
    kVampireUnit_2,
    
    // Werewolf
    kWerewolfUnit_0,
    kWerewolfUnit_1,
    kWerewolfUnit_2,
};

// Hero
enum
{
    // Vampire
    kVampireHero_0,
    kVampireHero_1,
    
    // Werewolf
    kWerewolfHero_0,
    kWerewolfHero_1,
    
    
};

// Tower
enum
{
    kVampireTower_0,
};

typedef enum
{
    kAttackType_Normal,
    
}AttackType;

#endif /* Global_h */
