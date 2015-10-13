//
//  CCShake.cpp
//  magicalbook
//
//  Created by Andy on 14-8-26.
//
//

#include "CCShake.h"

CCShake::CCShake()
:_strength_x(0)
,_strength_y(0)
,_initial_x(0)
,_initial_y(0)
{
}

CCShake* CCShake::actionWithDuration(float d, float strength, Point originalPosition )
{
	// call other construction method with twice the same strength
	return actionWithDuration( d, strength, strength, originalPosition );
}

CCShake* CCShake::actionWithDuration(float duration, float strength_x, float strength_y, Point originalPosition)
{
    CCShake *p_action = new (std::nothrow) CCShake();
	p_action->initWithDuration(duration, strength_x, strength_y, originalPosition);
	p_action->autorelease();
    
	return p_action;
}

bool CCShake::initWithDuration(float duration, float strength_x, float strength_y, Point originalPosition)
{
	if (ActionInterval::initWithDuration(duration))
	{
		_strength_x = strength_x;
		_strength_y = strength_y;
        _originalPosition = originalPosition;
		return true;
	}
    
	return false;
}

// Helper function. I included it here so that you can compile the whole file
// it returns a random value between min and max included
float fgRangeRand( float min, float max )
{
	float rnd = ((float)rand()/(float)RAND_MAX);
	return rnd*(max-min)+min;
}

void CCShake::update(float time)
{
	float randx = fgRangeRand( -_strength_x, _strength_x );
	float randy = fgRangeRand( -_strength_y, _strength_y );
    
	// move the target to a shaked position
	_target->setPosition( _originalPosition + Point( randx, randy));
}

void CCShake::startWithTarget(Node *pTarget)
{
	ActionInterval::startWithTarget( pTarget );
    
	// save the initial position
	_initial_x = _originalPosition.x;
	_initial_y = _originalPosition.y;
}

void CCShake::stop(void)
{
	// Action is done, reset clip position
	_target->setPosition(_originalPosition);
    
	ActionInterval::stop();
}