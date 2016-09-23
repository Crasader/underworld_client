//
//  LanguageNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/7/25.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "LanguageNode.h"
#include "CocosUtils.h"

using namespace std;

LanguageNode* LanguageNode::create()
{
    LanguageNode *ret = new (nothrow) LanguageNode();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

LanguageNode::LanguageNode()
:_button(nullptr)
,_observer(nullptr)
,_tick(nullptr)
,_touchInvalid(false)
,_idx(CC_INVALID_INDEX) {}

LanguageNode::~LanguageNode()
{
    removeAllChildren();
}

void LanguageNode::registerObserver(LanguageNodeObserver *observer)
{
    _observer = observer;
}

bool LanguageNode::init()
{
    if (Node::init())
    {
        static const string file(CocosUtils::getResourcePath("ui_tiao_7.png"));
        auto button = ui::Button::create(file, file);
        addChild(button);
        
        const auto& size(button->getContentSize());
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        button->setPosition(Point(size.width / 2, size.height / 2));
        
        static const float space(3);
        _tick = Sprite::create(CocosUtils::getResourcePath("icon_duihao.png"));
        const auto& tsize(_tick->getContentSize());
        _tick->setPosition(Point(space + tsize.width / 2, size.height / 2));
        _tick->setVisible(false);
        button->addChild(_tick);
        
        button->setTitleFontName(DEFAULT_FONT);
        button->setTitleFontSize(DEFAULT_FONT_SIZE);
        button->setTitleColor(Color3B::BLACK);
        button->setTitleAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
        
        CocosUtils::fixWidgetTouchEvent(button, _touchInvalid, nullptr, [this](Ref*) {
            if (_observer) {
                _observer->onLanguageNodeSelected(_idx);
            }
        });
        
        _button = button;
        
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

void LanguageNode::setIdx(ssize_t idx)
{
    _idx = idx;
}
