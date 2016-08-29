//
//  RuneNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "RuneNode.h"
#include "RuneData.h"
#include "RuneGroupData.h"
#include "CocosUtils.h"
#include "DevelopUI.h"

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
,_data(nullptr)
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
        
        auto bg = Sprite::create(DevelopUI::getResourcePath("ui_kuang_17.png"));
        addChild(bg);
        
        const auto& size(bg->getContentSize());
        setContentSize(size);
        bg->setPosition(size.width / 2, size.height / 2);
        
        _icon = Sprite::create(DevelopUI::getResourcePath("icon_fuwen_2.png"));
        _icon->setPosition(size.width / 2, size.height / 2);
        bg->addChild(_icon);
        
        static const float ledge(3);
        auto label = CocosUtils::createLabel("1", DEFAULT_FONT_SIZE, DEFAULT_NUMBER_FONT);
        label->setAlignment(TextHAlignment::RIGHT, TextVAlignment::CENTER);
        label->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
        label->setPosition(size.width - ledge, ledge + label->getContentSize().height / 2);
        bg->addChild(label);
        _level = label;
        
        label = CocosUtils::createLabel("1", DEFAULT_FONT_SIZE, DEFAULT_NUMBER_FONT);
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
        
        if (_level) {
            _level->setString(data ? StringUtils::format("LV.%d", data->getLevel()) : "");
        }
        
        if (_amount) {
            if (data) {
                auto groupData(dynamic_cast<const RuneGroupData*>(data));
                _amount->setVisible(groupData);
                if (groupData) {
                    auto amount(groupData->getAmount());
                    _amount->setString(StringUtils::format("x %d", amount));
                }
            } else {
                _amount->setString("");
            }
        }
    }
}

const RuneData* RuneNode::getData() const
{
    return _data;
}
