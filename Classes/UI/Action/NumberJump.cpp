//
//  NumberJump.cpp
//  Underworld_Client
//
//  Created by burst on 15-1-28.
//
//

#include "NumberJump.h"
#include "CocosUtils.h"
#include "2d/CCTweenFunction.h"

NumberJump* NumberJump::create(float duration, int startNum, int endNum, std::function<void(std::string)> callback)
{
    NumberJump *ret = new (std::nothrow) NumberJump();
    ret->initWithDuration(duration, startNum, endNum, callback);
    ret->autorelease();
    
    return ret;
}

bool NumberJump::initWithDuration(float duration, int startNum, int endNum, std::function<void(std::string)> callback)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _startNum = startNum;
        _endNum = endNum;
        _callBack = callback;
        return true;
    }
    
    return false;
}

NumberJump* NumberJump::clone() const
{
    // no copy constructor
    auto a = new (std::nothrow) NumberJump();
    a->initWithDuration(_duration, _startNum, _endNum, _callBack);
    a->autorelease();
    return a;
}

void NumberJump::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
    _previousNum = _startNum;
}

void NumberJump::update(float t)
{
    if (_target)
    {
        float t2 = tweenfunc::expoEaseOut(t);
        int num = _previousNum + floor((_endNum - _startNum) * t2);
        _callBack(CocosUtils::itoa(num));
    }
}