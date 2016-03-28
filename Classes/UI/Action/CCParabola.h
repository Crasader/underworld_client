//
//  CCParabola.h
//  Underworld_Client
//
//  Created by Andy on 16/3/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef CCParabola_h
#define CCParabola_h

#include "cocos2d.h"

USING_NS_CC;

class Parabola : public ActionInterval
{
public:
    static Parabola* create(float duration, const Point& startPoint, const Point& endPoint, float height);
    virtual void update(float time) override;
    
protected:
    Parabola();
    bool init(float duration, const Point& startPoint, const Point& endPoint, float height);
    
protected:
    Point _startPoint;
    Point _endPoint;
    float _width;
    float _height;
    
    float _a;
    float _b;
    float _c;
};

#endif /* CCParabola_h */
