//
//  TalentBriefNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/6/28.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "TalentBriefNode.h"
#include "CocosUtils.h"

using namespace std;

TalentBriefNode* TalentBriefNode::create()
{
    TalentBriefNode *ret = new (nothrow) TalentBriefNode();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

TalentBriefNode::TalentBriefNode()
:_name(nullptr)
,_level(nullptr)
,_brief(nullptr)
{
    
}

TalentBriefNode::~TalentBriefNode()
{
    removeAllChildren();
}

bool TalentBriefNode::init()
{
    if (Node::init()) {
        auto sprite = Sprite::create(CocosUtils::getResourcePath("ui_white.png"));
        addChild(sprite);
        
        const auto& size = sprite->getContentSize();
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        sprite->setPosition(Point(size.width / 2, size.height / 2));
        
        _name = CocosUtils::createLabel("name", DEFAULT_FONT_SIZE);
        sprite->addChild(_name);
        
        _level = CocosUtils::createLabel("level", DEFAULT_FONT_SIZE);
        sprite->addChild(_level);
        
        const auto& nameSize = _name->getContentSize();
        const auto& levelSize = _level->getContentSize();
        static const float offset(5.0f);
        const float edge = (size.width - (nameSize.width + offset + levelSize.width)) / 2;
        const float y = size.height - nameSize.height / 2;
        _name->setPosition(Point(edge + nameSize.width / 2, y));
        _level->setPosition(Point(size.width - (edge + levelSize.width / 2), y));
        
        const Size briefSize(size.width, size.height - nameSize.height);
        _brief = CocosUtils::createLabel("brief", DEFAULT_FONT_SIZE, DEFAULT_FONT, briefSize, TextHAlignment::CENTER, TextVAlignment::TOP);
        sprite->addChild(_brief);
        _brief->setPosition(Point(size.width / 2, briefSize.height / 2));
        
        return true;
    }
    
    return false;
}

void TalentBriefNode::update(const string& name, int level, const string& brief)
{
    if (_name) {
        _name->setString(name);
    }
    
    if (_level) {
        _level->setString(StringUtils::format("LV %d", level));
    }
    
    if (_brief) {
        _brief->setString(brief);
    }
}