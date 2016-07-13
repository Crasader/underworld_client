//
//  AnimationParameter.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "AnimationParameter.h"

AnimationParameter::AnimationParameter(const AnimationParameter* instance)
:scale(1.0f)
,speed(1.0f)
,atkIdx(0)
,castIdx(0)
{
    if (instance) {
        scale = instance->scale;
        speed = instance->speed;
        atkIdx = instance->atkIdx;
        castIdx = instance->castIdx;
    }
}

AnimationParameter::~AnimationParameter() {}
