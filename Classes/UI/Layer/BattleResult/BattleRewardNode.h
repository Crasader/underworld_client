//
//  BattleRewardNode.h
//  Underworld_Client
//
//  Created by Andy on 16/9/20.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef BattleRewardNode_h
#define BattleRewardNode_h

#include "cocos2d.h"
#include "CocosGlobal.h"

USING_NS_CC;

class BattleRewardNode : public Node
{
public:
    static BattleRewardNode* create();
    virtual ~BattleRewardNode();
    
    void setTrophy(int count);
    void setResource(ResourceType type, int count);
    
private:
    BattleRewardNode();
    virtual bool init() override;
    void resize();
    
private:
    Sprite* _icon;
    Label* _count;
};

#endif /* BattleRewardNode_h */
