//
//  GameScheduler.h
//  Underworld_Client
//
//  Created by burst on 15/10/30.
//
//

#ifndef GameScheduler_h
#define GameScheduler_h

#include <stdio.h>
#include "cocos2d.h"
#include "ExternalInterface.h"

USING_NS_CC;

class GameScheduler: public UnderWorld::Core::AbstractScheduler
{
public:
    GameScheduler(Scheduler* cocosScheduler = Director::getInstance()->getScheduler());
    virtual ~GameScheduler();
    /** schedule a function call */
    virtual void schedule(int fps,
                         const std::function<void ()>& call,
                         const std::string& key);
    
    /** unSchedule a function call */
    virtual void unSchedule(const std::string& key);
private:
    Scheduler* _cocosScheduler;
    
};
#endif /* GameScheduler_h */
