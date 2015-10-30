//
//  GameScheduler.cpp
//  Underworld_Client
//
//  Created by burst on 15/10/30.
//
//

#include "GameScheduler.h"

using namespace UnderWorld::Core;

GameScheduler::GameScheduler(Scheduler* cocosScheduler)
:_cocosScheduler(cocosScheduler)
{
    
}

void GameScheduler::schedule(int fps,
                            const std::function<void ()>& call,
                            const std::string& key)
{
    _cocosScheduler->schedule([call](float){
        call();
    }, this, 1.0f / fps, false, key);
}

void GameScheduler::unSchedule(const std::string &key)
{
    _cocosScheduler->unschedule(key, this);
}
