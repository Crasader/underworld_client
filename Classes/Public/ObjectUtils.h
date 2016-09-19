//
//  ObjectUtils.h
//  Underworld_Client
//
//  Created by Andy on 16/9/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef ObjectUtils_h
#define ObjectUtils_h

namespace ObjectUtils
{
    enum class Type {
        NONE = 0,
        CARD = 2,
        SKILL = 7,
        RUNE = 8,
        BOOK = 9,
    };
    Type getType(int oid);
    
    enum class RuneType {
        NONE = -1,
        YELLOW = 0,
        BLUE,
        GREEN
    };
    RuneType getRuneType(int oid);
    
    enum class CardAttributeType {
        HP,
        SPEED,
        ARMOR,
        ARMOR_TYPE,
        GROUND_DAMAGE,
        ATTACK_TYPE,
        HIT_SPEED,
        RANGE,
        TARGET_TYPE,
        AIR_DAMAGE,
        
        PROPERTY_MAX,
    };
    
    enum class AttackType {
        NORMAL,
        PIERCING,
        MAGIC,
        HERO,
        SIEGE,
        CHAOS,
    };
    
    enum class ArmorType {
        HEAVY,
        MEDIUM,
        UNARMORED,
        HERO,
        FORTIFIED,
        LIGHT,
    };
    
    enum class TargetType {
        GROUND,
        AIR,
        BOTH,
    };
}

#endif /* ObjectUtils_h */
