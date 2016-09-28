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
#include "BattleResultData.h"
#include "ObjectBriefData.h"

using namespace std;

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
,_userNode(nullptr)
,_trophy(nullptr)
,_data(nullptr) {}

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
        addChild(result, zorder_top);
        _result = result;
        
        auto flash = Sprite::create(CocosUtils::getResourcePath("texiao_guang.png"));
        flash->setPosition(result->getPosition() - Point(0, 75));
        addChild(flash);
        
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
        addChild(user, zorder_top);
        _userNode = user;
        
        auto trophy(BattleRewardNode::create());
        trophy->setTrophy();
        addChild(trophy, zorder_top);
        _trophy = trophy;
        
        static const int rewardsCount(2);
        for (int i = 0; i < rewardsCount; ++i) {
            auto node = BattleRewardNode::create();
            if (0 == i) {
                node->setResource(ResourceType::Gem);
            } else if (1 == i) {
                node->setResource(ResourceType::Gold);
            }
            addChild(node, zorder_top);
            _rewards.push_back(node);
        }
        
        static const float urspace(20);
        trophy->setPosition(winSize.width / 2, user->getPositionY() - (usize.height / 2 + urspace + trophy->getContentSize().height / 2));
        static const Vec2 rspace(80, 80);
        _rewards.at(0)->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
        _rewards.at(0)->setPosition(winSize.width / 2 - rspace.x / 2, trophy->getPositionY() - rspace.y);
        
        _rewards.at(1)->setPosition(Point::ANCHOR_MIDDLE_LEFT);
        _rewards.at(1)->setPosition(winSize.width / 2 + rspace.x / 2, _rewards.at(0)->getPositionY());
        
        static const float bedge(110);
        auto button = XButton::create(XButton::BSize::Big, XButton::BType::Blue);
        button->setTitleText(LocalHelper::getString("hint_confirm"));
        button->setPosition(Point(winSize.width / 2, bedge + button->getContentSize().height / 2));
        button->addClickEventListener([this](Ref*) {
            removeFromParent();
            if (_observer) {
                _observer->onBattleResultLayerConfirm(this);
            }
        });
        addChild(button, zorder_top);
        
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

void BattleResultLayer::update(const BattleResultData* data)
{
    if (_data != data) {
        _data = data;
        
        // TODO:
        if (/* DISABLES CODE */ (false) && _result) {
            _result->setTexture("");
        }
        
        if (_trophy) {
            _trophy->setCount(data->getTrophy());
        }
        
        auto rewards(data->getRewards());
        const auto cnt(_rewards.size());
        for (int i = 0; i < rewards.size(); ++i) {
            if (i < cnt) {
                auto data(rewards.at(i));
                auto node(_rewards.at(i));
                node->setVisible(true);
                node->setResource(static_cast<ResourceType>(data->getId()));
                node->setCount(data->getCount());
            }
        }
        
        for (auto i = rewards.size(); i < cnt; ++i) {
            _rewards.at(i)->setVisible(false);
        }
    }
}
