//
//  ObjectUtils.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "ObjectUtils.h"

ObjectUtils::Type ObjectUtils::getType(int oid)
{
    return static_cast<Type>(oid / 10000);
}

ObjectUtils::RuneType ObjectUtils::getRuneType(int oid) {
    if (Type::RUNE == getType(oid)) {
        return static_cast<RuneType>((oid % 10000) / 1000);
    }
    
    return RuneType::NONE;
}
