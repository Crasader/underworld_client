//
//  ResourceNode.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/15.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ResourceNode.h"
#include "ui/CocosGUI.h"
#include "CocosUtils.h"

using namespace std;
using namespace ui;

ResourceNode::ResourceNode()
:_type(ResourceType::Gold)
,_count(INVALID_VALUE)
,_callback(nullptr)
,_countLabel(nullptr)
{
    
}

ResourceNode::~ResourceNode()
{
    removeFromParent();
}

ResourceNode* ResourceNode::create(ResourceType type, int count, const Callback& callback)
{
    ResourceNode *p = new (nothrow) ResourceNode();
    if(p && p->init(type, count, callback)) {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool ResourceNode::init(ResourceType type, int count, const Callback& callback)
{
    if(Node::init())
    {
        _type = type;
        _count = count;
        _callback = callback;
        
        Sprite* background(nullptr);
        if (ResourceType::Stamina == type) {
            background = Sprite::create("GameImages/main_ui/ui_tiao_3.png");
            
            auto inside = Sprite::create("GameImages/main_ui/ui_tiao_3_3.png");
            const auto& size(background->getContentSize());
            inside->setPosition(Point(size.width / 2, size.height / 2) + Point(-3, 2));
            background->addChild(inside);
        } else {
            background = Sprite::create("GameImages/main_ui/ui_tiao_4.png");
        }
        
        if (background) {
            addChild(background);
            
            auto icon = Sprite::create(StringUtils::format("GameImages/resources/icon_%dB.png", type));
            addChild(icon);
            
            _countLabel = CocosUtils::createLabel(StringUtils::format("%d", count), DEFAULT_FONT_SIZE, DEFAULT_NUMBER_FONT, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
            _countLabel->setAnchorPoint(Point::ANCHOR_MIDDLE);
            addChild(_countLabel);
            
            Button* button(nullptr);
            if (nullptr != callback) {
                static const string file("GameImages/main_ui/button_lvse.png");
                button = Button::create(file, file);
                button->setPressedActionEnabled(true);
                button->addClickEventListener([this](Ref*) {
                    if (_callback) {
                        _callback(this);
                    }
                });
                addChild(button);
            }
            
            const auto& bgsize(background->getContentSize());
            const auto& isize(icon->getContentSize());
            const auto& bsize = button ? (button->getContentSize()) : Size::ZERO;
            const Size size = bgsize + Size((isize.width + bsize.width) / 2, 0);
            setAnchorPoint(Point::ANCHOR_MIDDLE);
            setContentSize(size);
            
            const float y = size.height / 2;
            icon->setPosition(Point(isize.width / 2, y));
            background->setPosition(Point((isize.width + bgsize.width) / 2, y));
            _countLabel->setPosition(Point(isize.width + (bgsize.width - (bsize.width + isize.width) / 2) / 2, y + 3));
            if (button) {
                button->setPosition(Point(size.width - bsize.width / 2, y));
            }
        }
        
        return true;
    }
    
    return false;
}

ResourceType ResourceNode::getType() const
{
    return _type;
}

int ResourceNode::getCount() const
{
    return _count;
}

void ResourceNode::setCount(int count, bool animated)
{
    _count = count;
    
    if (_countLabel)
    {
        if (animated) {
            static const float duration(0.5f);
            CocosUtils::jumpNumber(_countLabel, count, duration);
        } else {
            _countLabel->setString(StringUtils::format("%d", count));
        }
    }
}
