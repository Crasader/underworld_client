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
}

#endif /* ObjectUtils_h */
