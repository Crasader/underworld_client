//
//  UnderworldTestClient.h
//  Underworld_Client
//
//  Created by Mac on 16/3/29.
//
//

#ifndef UnderworldTestPvpClient_h
#define UnderworldTestPvpClient_h

#include "UnderworldClient.h"
#include "GameScheduler.h"

class UnderworldTestPvpClient {
private:
    UnderworldClient* _playerA;
    UnderworldClient* _playerB;
    GameScheduler* _sch;
    
    
public:
    UnderworldTestPvpClient();
    
    void init(UnderWorld::Core::AbstractRender* render);
    void startTest(const UnderWorld::Core::GameContentSetting& setting);
    void endTest();
    
};

#endif /* UnderworldTestPvpClient_h */
