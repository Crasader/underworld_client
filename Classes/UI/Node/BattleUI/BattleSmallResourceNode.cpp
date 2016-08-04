//
//  BattleSmallResourceNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/2/27.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "BattleSmallResourceNode.h"
#include "CocosUtils.h"

using namespace std;

BattleSmallResourceNode::BattleSmallResourceNode()
:_type(ResourceType::Gold)
,_count(0)
,_icon(nullptr)
,_countLabel(nullptr)
{
    
}

BattleSmallResourceNode::~BattleSmallResourceNode()
{
    removeFromParent();
}

BattleSmallResourceNode* BattleSmallResourceNode::create(ResourceType type, int count)
{
    BattleSmallResourceNode *p = new (nothrow) BattleSmallResourceNode();
    if(p && p->init(type, count))
    {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool BattleSmallResourceNode::init(ResourceType type, int count)
{
    if(Node::init())
    {
        _type = type;
        _count = count;
        
        string file;
        if (ResourceType::Gold == type) {
            file = "UI-quan-2.csb";
        } else if (ResourceType::Wood == type) {
            file = "UI-quan-1.csb";
        }
        Node *effect = CocosUtils::playAnimation(file, 0, true);
        addChild(effect);
        _icon = effect;
        
        const Size size(effect->getChildren().front()->getContentSize());
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        effect->setPosition(Point(size.width / 2, size.height / 2));
        
        _countLabel = CocosUtils::createLabel(StringUtils::format("%d", count), DEFAULT_FONT_SIZE, DEFAULT_NUMBER_FONT);
        _countLabel->setPosition(effect->getPosition());
        addChild(_countLabel, 1);
        
        return true;
    }
    
    return false;
}

void BattleSmallResourceNode::setOpacity(GLubyte opacity)
{
    Node::setOpacity(opacity);
    
    if (_icon) {
        Vector<Node*>& children = _icon->getChildren();
        if (children.size() > 0) {
            Sprite* sprite = dynamic_cast<Sprite*>(children.front());
            if (sprite) {
                sprite->setOpacity(opacity);
            }
        }
    }
    
    if (_countLabel) {
        _countLabel->setOpacity(opacity);
    }
}

void BattleSmallResourceNode::check(int count)
{
    if (_countLabel) {
        const Color4B& color = (count >= _count) ? ENABLED_COLOR : DISABLED_COLOR;
        if (_countLabel->getTextColor() != color) {
            _countLabel->setTextColor(color);
        }
    }
}

void BattleSmallResourceNode::setCount(int count)
{
    if (count != _count) {
        _count = count;
        
        if (_countLabel) {
            _countLabel->setString(std::to_string(count));
        }
    }
}

ResourceType BattleSmallResourceNode::getType() const
{
    return _type;
}

int BattleSmallResourceNode::getCount() const
{
    return _count;
}
