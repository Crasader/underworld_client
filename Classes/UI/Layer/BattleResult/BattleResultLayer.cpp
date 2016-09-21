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
#include "LocalHelper.h"
#include "UserSimpleNode.h"
#include "BattleRewardNode.h"
#include "XButton.h"

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
        flash->setPosition(result->getPosition() - Point(0, 75));
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
        
        static const int rewardsCount(3);
        for (int i = 0; i < rewardsCount; ++i) {
            auto node = BattleRewardNode::create();
            if (0 == i) {
                node->setTrophy();
            } else if (1 == i) {
                node->setResource(ResourceType::Gem);
            } else if (2 == i) {
                node->setResource(ResourceType::Gold);
            }
            addChild(node);
            _rewards.push_back(node);
        }
        
        static const float urspace(20);
        _rewards.at(0)->setPosition(winSize.width / 2, user->getPositionY() - (usize.height / 2 + urspace + _rewards.at(0)->getContentSize().height / 2));
        static const Vec2 rspace(80, 80);
        _rewards.at(1)->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
        _rewards.at(1)->setPosition(winSize.width / 2 - rspace.x / 2, _rewards.at(0)->getPositionY() - rspace.y);
        
        _rewards.at(2)->setPosition(Point::ANCHOR_MIDDLE_LEFT);
        _rewards.at(2)->setPosition(winSize.width / 2 + rspace.x / 2, _rewards.at(1)->getPositionY());
        
        static const float bedge(110);
        auto button = XButton::create(XButton::BSize::Big, XButton::BType::Blue);
        button->setTitleText(LocalHelper::getString("hint_confirm"));
        button->setPosition(Point(winSize.width / 2, bedge + button->getContentSize().height / 2));
        addChild(button);
        
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
