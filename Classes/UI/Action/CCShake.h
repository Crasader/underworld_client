//
//  CCShake.h
//  magicalbook
//
//  Created by Andy on 14-8-26.
//
//

#ifndef CCShake__
#define CCShake__

#include "cocos2d.h"

USING_NS_CC;

class CCShake : public ActionInterval
{
public:
	CCShake();
    
	// Create the action with a time and a strength (same in x and y)
	static CCShake* actionWithDuration(float d, float strength, Point originalPosition );
	// Create the action with a time and strengths (different in x and y)
	static CCShake* actionWithDuration(float d, float strength_x, float strength_y , Point originalPosition);
	bool initWithDuration(float d, float strength_x, float strength_y, Point originalPosition );
    
	virtual void startWithTarget(Node *pTarget);
	virtual void update(float time);
	virtual void stop(void);
    
protected:
	// Initial position of the shaked node
	float _initial_x, _initial_y;
	// Strength of the action
	float _strength_x, _strength_y;
    
    Point _originalPosition;
};

#endif /* defined(CCShake__) */
