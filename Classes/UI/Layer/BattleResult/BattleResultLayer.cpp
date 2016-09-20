//
//  BattleResultLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/20.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "BattleResultLayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "UserSimpleNode.h"
#include "BattleRewardNode.h"

using namespace std;

static const int zorder_bottom(-1);
static const int zorder_top(1);

BattleResultLayer* BattleResultLayer::create()
{
    auto ret = new (nothrow) BattleResultLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

BattleResultLayer::BattleResultLayer()
:_observer(nullptr)
,_result(nullptr)
,_userNode(nullptr) {}

BattleResultLayer::~BattleResultLayer()
{
    removeAllChildren();
}

bool BattleResultLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        static float edge(70);
        auto result = Sprite::create(CocosUtils::getResourcePath("zi_shengli.png"));
        const auto& rsize(result->getContentSize());
        result->setPosition(winSize.width / 2, winSize.height - (edge + rsize.height / 2));
        addChild(result);
        _result = result;
        
        auto flash = Sprite::create(CocosUtils::getResourcePath("texiao_guang.png"));
        flash->setPosition(result->getPosition());
        addChild(flash, zorder_bottom);
        
        auto fireworks = Sprite::create(CocosUtils::getResourcePath("texiao_caidan.png"));
        const auto& fsize(fireworks->getContentSize());
        fireworks->setPosition(winSize.width / 2, winSize.height - fsize.height / 2);
        addChild(fireworks, zorder_top);
        
        static const float ruspace(40);
        auto user = UserSimpleNode::create();
        user->setIsHome(true);
        user->setIsMe(false);
        const auto& usize(user->getContentSize());
        user->setPosition(winSize.width / 2, result->getPositionY() - (rsize.height + usize.height) / 2 - ruspace);
        addChild(user);
        _userNode = user;
        
        for (int i = 0; i < 3; ++i) {
            auto node = BattleRewardNode::create();
            node->setPosition(winSize.width / 2, winSize.height / 2);
            addChild(node);
        }
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(BattleResultLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(BattleResultLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool BattleResultLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void BattleResultLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

void BattleResultLayer::registerObserver(BattleResultLayerObserver *observer)
{
    _observer = observer;
}
