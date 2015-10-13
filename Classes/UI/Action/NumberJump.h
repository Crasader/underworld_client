//
//  NumberJump.h
//  Underworld_Client
//
//  Created by burst on 15-1-28.
//
//

#ifndef NumberJump__
#define NumberJump__

#include <stdio.h>
#include "cocos2d.h"

USING_NS_CC;

#define NUMBERJUMP_ACTION_TAG (999)

class NumberJump : public ActionInterval
{
public:
    /** creates the action */
    static NumberJump* create(float duration, int startNum, int endNum, std::function<void(std::string)> callback);
    
    //
    // Overrides
    //
    virtual NumberJump* clone() const override;
    virtual void startWithTarget(Node *target) override;
    virtual void update(float time) override;
    
CC_CONSTRUCTOR_ACCESS:
    NumberJump() {}
    virtual ~NumberJump() {}
    
    /** initializes the action */
    bool initWithDuration(float duration, int startNum, int endNum, std::function<void(std::string)> callback);
    
protected:
    int _endNum;
    int _startNum;
    int _previousNum;
    
private:
    CC_DISALLOW_COPY_AND_ASSIGN(NumberJump);
    std::function<void(std::string)> _callBack;
};

#endif /* defined(NumberJump__) */
