//
//  BattleBattleResourceNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/2/18.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "BattleResourceNode.h"
#include "CocosUtils.h"

using namespace std;

static string getProgressTimerSprite(ResourceType type)
{
    if (ResourceType::Gold == type) {
        return "GameImages/test/button_5_2.png";
    } else if (ResourceType::Wood == type) {
        return "GameImages/test/button_5_1.png";
    }
    
    return "";
}

BattleResourceNode::BattleResourceNode()
:_type(ResourceType::Gold)
,_count(0)
,_subCount(0)
,_icon(nullptr)
,_countLabel(nullptr)
,_subCountLabel(nullptr)
,_progressTimer(nullptr)
{
    
}

BattleResourceNode::~BattleResourceNode()
{
    removeFromParent();
}

BattleResourceNode* BattleResourceNode::create(ResourceType type)
{
    BattleResourceNode *p = new (nothrow) BattleResourceNode();
    if(p && p->init(type))
    {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool BattleResourceNode::init(ResourceType type)
{
    if(Node::init())
    {
        Sprite* bg = Sprite::create("GameImages/test/button_5.png");
        addChild(bg);
        
        _progressTimer = ProgressTimer::create(Sprite::create("GameImages/test/button_5_1.png"));
        _progressTimer->setType(ProgressTimer::Type::RADIAL);
        _progressTimer->setReverseDirection(true);
        _progressTimer->setMidpoint(Vec2(0.5f, 0.5f));
        _progressTimer->setPercentage(60);
        addChild(_progressTimer);
        
        _icon = Sprite::create(StringUtils::format("GameImages/resources/icon_%dB.png", type));
        addChild(_icon);
        
        _countLabel = CocosUtils::createLabel("0", 36, DEFAULT_NUMBER_FONT);
        addChild(_countLabel);
        
        _subCountLabel = CocosUtils::createLabel("0.60", 18, DEFAULT_NUMBER_FONT);
        addChild(_subCountLabel);
        
        string file;
        if (ResourceType::Gold == type) {
            file = "UI-number-hong.csb";
        } else if (ResourceType::Wood == type) {
            file = "UI-number-lan.csb";
        }
        Node *effect = CocosUtils::playAnimation(file, 0, true);
        addChild(effect);
        
        // set content size
        const Size& pSize = _progressTimer->getContentSize();
        setContentSize(pSize + Size(0, 18 + _subCountLabel->getContentSize().height / 2));
        
        // set positions
        const Size& contentSize = getContentSize();
        const Point mid(contentSize.width / 2, contentSize.height / 2);
        bg->setPosition(mid);
        _progressTimer->setPosition(mid);
        _icon->setPosition(mid + Point(0, pSize.height / 2 - 5.0f));
        _countLabel->setPosition(mid);
        _subCountLabel->setPosition(mid - Point(0, pSize.height / 2));
        effect->setPosition(mid);
        
        setType(type);
        
        return true;
    }
    
    return false;
}

ResourceType BattleResourceNode::getType() const
{
    return _type;
}

int BattleResourceNode::getCount() const
{
    return _count;
}

float BattleResourceNode::getSubCount() const
{
    return _subCount;
}

void BattleResourceNode::setType(ResourceType type)
{
    _type = type;
    
    if (_progressTimer) {
        _progressTimer->setSprite(Sprite::create(getProgressTimerSprite(type)));
    }
    
    if (_icon) {
        string fileName = StringUtils::format("GameImages/resources/icon_%dB.png", type);
        assert(FileUtils::getInstance()->isFileExist(fileName));
        _icon->setTexture(fileName);
    }
}

void BattleResourceNode::setCount(int count, bool animated)
{
    _count = count;
    
    if (_countLabel) {
        if (animated) {
            static const float duration(0.5f);
            CocosUtils::jumpNumber(_countLabel, count, duration);
        } else {
            _countLabel->setString(StringUtils::format("%d", count));
        }
    }
}

void BattleResourceNode::setSubCount(float count)
{
    _subCount = count;
    
    if (_subCountLabel) {
        _subCountLabel->setString(StringUtils::format("%.2f", count));
    }
    
    if (_progressTimer) {
        _progressTimer->setPercentage(100.0f * count / 1.0f);
    }
}
