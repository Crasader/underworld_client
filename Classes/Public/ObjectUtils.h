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
    inline Type getType(int oid) { return static_cast<Type>(oid / 10000); }
    
    enum class RuneType {
        NONE = -1,
        YELLOW = 0,
        BLUE,
        GREEN
    };
    inline RuneType getRuneType(int oid) {
        if (Type::RUNE == getType(oid)) {
            return static_cast<RuneType>((oid % 10000) / 1000);
        }
        
        return RuneType::NONE;
    }
}

#endif /* ObjectUtils_h */
