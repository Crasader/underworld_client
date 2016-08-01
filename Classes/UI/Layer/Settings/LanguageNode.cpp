//
//  LanguageNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/7/25.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "LanguageNode.h"
#include "CocosUtils.h"
#include "SettingUI.h"

using namespace std;

LanguageNode* LanguageNode::create(const string& name)
{
    LanguageNode *ret = new (nothrow) LanguageNode();
    if (ret && ret->init(name))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

LanguageNode::LanguageNode()
:_button(nullptr)
,_observer(nullptr) {}

LanguageNode::~LanguageNode()
{
    removeAllChildren();
}

void LanguageNode::registerObserver(LanguageNodeObserver *observer)
{
    _observer = observer;
}

bool LanguageNode::init(const string& name)
{
    if (Node::init())
    {
        static const string file(SettingUI::getResourcePath("ui_tiao_7.png"));
        auto button = ui::Button::create(file, file);
        addChild(button);
        
        const auto& size(button->getContentSize());
        static const float space(3);
        _tick = Sprite::create(SettingUI::getResourcePath("icon_duihao.png"));
        const auto& tsize(_tick->getContentSize());
        _tick->setPosition(Point(space + tsize.width / 2, size.height / 2));
        _tick->setVisible(false);
        button->addChild(_tick);
        
        button->setTitleFontName(DEFAULT_FONT);
        button->setTitleFontSize(DEFAULT_FONT_SIZE);
        button->setTitleColor(Color3B::BLACK);
        button->setTitleAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
        
        auto label = button->getTitleRenderer();
        if (label) {
            label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            label->setPosition(_tick->getPosition() + Point(tsize.width / 2 + space, 0));
        }
        
        update(name);
        
        return true;
    }
    
    return false;
}

void LanguageNode::tick(bool ticked)
{
    if (_tick) {
        _tick->setVisible(ticked);
    }
}

void LanguageNode::update(const string& name)
{
    if (_button) {
        _button->setTitleText(name);
    }
}
