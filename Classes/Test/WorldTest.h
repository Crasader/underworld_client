//
//  WorldTest.hpp
//  Underworld_Client
//
//  Created by wenchengye on 16/5/20.
//
//

#ifndef WorldTest_h
#define WorldTest_h

#include "World.h"

class WorldTest : public UnderWorld::Core::WorldObserver {
public:
    void test();
    
    virtual void onNotifyWorldEvents(const std::vector<UnderWorld::Core::World::EventLog>& events);
};

#endif /* WorldTest_hpp */
