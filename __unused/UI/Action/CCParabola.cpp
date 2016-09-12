//
//  CCParabola.cpp
//  Underworld_Client
//
//  Created by Andy on 16/3/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "CCParabola.h"

Parabola* Parabola::create(float duration, const Point& startPoint, const Point& endPoint, float height)
{
    Parabola *ret = new (std::nothrow) Parabola();
    if (ret && ret->init(duration, startPoint, endPoint, height)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

Parabola::Parabola()
:_startPoint(Point::ZERO)
,_endPoint(Point::ZERO)
,_width(0)
,_height(0)
,_a(0)
,_b(0)
,_c(0)
{
    
}

bool Parabola::init(float duration, const Point& startPoint, const Point& endPoint, float height)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _startPoint = startPoint;
        _endPoint = endPoint;
        _height = height;
        
        const float x1 = _startPoint.x;
        const float y1 = _startPoint.y;
        const float x3 = _endPoint.x;
        const float y3 = _endPoint.y;
        
        _width = fabsf(x3 - x1);
        const float x2 = x1 + _width / 2;
        const float y2 = y1 + _height;
        
        /* equations
         * a * x1^2 + b * x1 + c = y1
         * a * x2^2 + b * x2 + c = y2
         * a * x3^2 + b * x3 + c = y3
         */
        
        _b = ((y1 - y3) * (pow(x1, 2) - pow(x2, 2)) - (y1 - y2) * (pow(x1, 2)-pow(x3, 2))) / ((x1 - x3) * (pow(x1, 2) - pow(x2, 2)) - (x1 - x2)*(pow(x1, 2) - pow(x3, 2)));
        _a = ((y1-y2) - _b * (x1 - x2)) / (pow(x1, 2) - pow(x2, 2));
        _c = y1 - _a * pow(x1, 2) - _b * x1;
        
        return true;
    }
    
    return false;
}

void Parabola::update(float time)
{
    const float x = _startPoint.x + time * _width;
    const float y = _a * pow(x, 2) + _b * x + _c;
    _target->setPosition(x, y);
}