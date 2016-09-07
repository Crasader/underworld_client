//
//  RuneNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "RuneNode.h"
#include "RuneData.h"
#include "CocosUtils.h"
#include "ObjectUtils.h"

using namespace std;

RuneNode* RuneNode::create(const RuneData* data)
{
    auto ret = new (nothrow) RuneNode();
    if (ret && ret->init(data)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

RuneNode::RuneNode()
:_observer(nullptr)
,_icon(nullptr)
,_level(nullptr)
,_amount(nullptr)
,_shiningSprite(nullptr)
,_data(nullptr)
,_idx(-1)
,_touchInvalid(false) {}

RuneNode::~RuneNode()
{
    removeAllChildren();
}

bool RuneNode::init(const RuneData* data)
{
    if (Widget::init())
    {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        
        auto bg = Sprite::create(CocosUtils::getResourcePath("ui_kuang_17.png"));
        addChild(bg);
        
        const auto& size(bg->getContentSize());
        setContentSize(size);
        bg->setPosition(size.width / 2, size.height / 2);
        
        _icon = Sprite::create();
        _icon->setPosition(size.width / 2, size.height / 2);
        bg->addChild(_icon);
        
        static const float ledge(3);
        auto label = CocosUtils::createLabel("", DEFAULT_FONT_SIZE, DEFAULT_NUMBER_FONT);
        label->setAlignment(TextHAlignment::RIGHT, TextVAlignment::CENTER);
        label->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
        label->setPosition(size.width - ledge, ledge + label->getContentSize().height / 2);
        bg->addChild(label);
        _level = label;
        
        label = CocosUtils::createLabel("", DEFAULT_FONT_SIZE, DEFAULT_NUMBER_FONT);
        label->setAlignment(TextHAlignment::RIGHT, TextVAlignment::CENTER);
        label->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
        label->setPosition(size.width - ledge, size.height - (ledge + label->getContentSize().height / 2));
        bg->addChild(label);
        _amount = label;

        setTouchEnabled(true);
        setSwallowTouches(false);
        CocosUtils::fixWidgetTouchEvent(this, _touchInvalid, nullptr, [this](Ref*) {
            if (_observer) {
                _observer->onRuneNodeClicked(this);
            }
        });
        
        {
            auto sprite = Sprite::create(CocosUtils::getResourcePath("ui_kuang_18.png"));
            sprite->setVisible(false);
            sprite->setPosition(size.width / 2, size.height / 2);
            bg->addChild(sprite);
            _shiningSprite = sprite;
        }
        
        update(data);
        
        return true;
    }
    
    return false;
}

void RuneNode::registerObserver(RuneNodeObserver *observer)
{
    _observer = observer;
}

void RuneNode::update(const RuneData* data)
{
    if (_data != data) {
        _data = data;
        
        if (_icon) {
            if (data) {
                const string file(StringUtils::format("GameImages/icons/rune/icon_fuwen_%d.png", static_cast<int>(ObjectUtils::getRuneType(data->getId())) + 1));
                _icon->setTexture(file);
            } else {
                _icon->initWithTexture(nullptr, Rect::ZERO);
            }
        }
        
        if (_amount) {
            _amount->setString(data && data->getAmount() > 0 ? StringUtils::format("x %d", data->getAmount()) : "");
        }
        
        if (_level) {
            _level->setString(data && data->getId() > 0 ? StringUtils::format("LV.%d", data->getLevel()) : "");
        }
    }
}

const RuneData* RuneNode::getData() const
{
    return _data;
}

void RuneNode::select(bool selected)
{
    if (_shiningSprite) {
        _shiningSprite->setVisible(selected);
    }
}

void RuneNode::showAmount(bool show)
{
    if (_amount) {
        _amount->setVisible(show);
    }
}

void RuneNode::setIdx(int idx)
{
    _idx = idx;
}

int RuneNode::getIdx() const
{
    return _idx;
}
