//
//  BattleResultLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/9/20.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef BattleResultLayer_h
#define BattleResultLayer_h

#include "cocos2d.h"

USING_NS_CC;

class UserSimpleNode;
class BattleRewardNode;
class BattleResultLayer;

class BattleResultLayerObserver
{
public:
    virtual ~BattleResultLayerObserver() {}
    virtual void onBattleResultLayerConfirm(BattleResultLayer* pSender) = 0;
};

class BattleResultLayer : public LayerColor
{
public:
    static BattleResultLayer* create();
    virtual ~BattleResultLayer();
    void registerObserver(BattleResultLayerObserver *observer);
    
private:
    BattleResultLayer();
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
private:
    BattleResultLayerObserver* _observer;
    Sprite* _result;
    UserSimpleNode* _userNode;
    std::vector<BattleRewardNode*> _rewards;
};

#endif /* BattleResultLayer_h */
