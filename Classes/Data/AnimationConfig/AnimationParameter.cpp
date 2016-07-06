//
//  AnimationParameter.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "AnimationParameter.h"

AnimationParameter::AnimationParameter()
:scale(1.0f)
,speed(1.0f) {}

AnimationParameter::AnimationParameter(const AnimationParameter& instance)
:scale(instance.scale)
,speed(instance.speed) {}

AnimationParameter::~AnimationParameter() {}
