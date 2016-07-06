//
//  AnimationParameter.h
//  Underworld_Client
//
//  Created by Andy on 15/10/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef AnimationParameter_h
#define AnimationParameter_h

class AnimationParameter
{
public:
    AnimationParameter();
    AnimationParameter(const AnimationParameter& instance);
    virtual ~AnimationParameter();
    
    float scale;
    float speed;
};

#endif /* AnimationParameter_h */
