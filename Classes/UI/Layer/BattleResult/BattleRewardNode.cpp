//
//  BattleRewardNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/20.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "BattleRewardNode.h"
#include "CocosUtils.h"

using namespace std;

static const int TrophyId(2016920);

BattleRewardNode* BattleRewardNode::create()
{
    auto ret = new (nothrow) BattleRewardNode();
    if(ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

BattleRewardNode::BattleRewardNode()
:_rewardId(0)
,_icon(nullptr)
,_count(nullptr) {}

BattleRewardNode::~BattleRewardNode()
{
    removeFromParent();
}

bool BattleRewardNode::init()
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        
        auto icon = Sprite::create(CocosUtils::getResourcePath("icon_jiangbei.png"));
        addChild(icon);
        _icon = icon;
        
        auto count = CocosUtils::createLabel("", 60, DEFAULT_NUMBER_FONT, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
        count->setAnchorPoint(Point::ANCHOR_MIDDLE);
        addChild(count);
        _count = count;
        
        resize();
        
        return true;
    }
    
    return false;
}

void BattleRewardNode::setTrophy()
{
    if (_rewardId != TrophyId) {
        _rewardId = TrophyId;
        
        if (_icon) {
            _icon->setTexture(CocosUtils::getResourcePath("icon_jiangbei.png"));
            resize();
        }
    }
}

void BattleRewardNode::setResource(ResourceType type)
{
    int rewardId(static_cast<int>(type));
    if (_rewardId != rewardId) {
        _rewardId = rewardId;
        
        if (_icon) {
            _icon->setTexture(StringUtils::format("GameImages/resources/icon_%dB.png", static_cast<int>(type)));
            resize();
        }
    }
}

void BattleRewardNode::setCount(int count)
{
    if (_count) {
        const string prefix((_rewardId == TrophyId) ? "+" : "x");
        _count->setString(prefix + " " + StringUtils::format("%d", count));
        resize();
    }
}

void BattleRewardNode::resize()
{
    static const float space(5);
    auto isize(_icon->getContentSize());
    auto lsize(_count->getContentSize());
    const Size size(isize.width + lsize.width + space, MAX(isize.height, lsize.height));
    if (!size.equals(_contentSize)) {
        setContentSize(size);
        _icon->setPosition(isize.width / 2, size.height / 2);
        _count->setPosition(size.width - lsize.width / 2, size.height / 2);
    }
}
