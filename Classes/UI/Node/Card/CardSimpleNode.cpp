//
//  CardSimpleNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardSimpleNode.h"
#include "CardSimpleData.h"
#include "CocosUtils.h"

using namespace std;

CardSimpleNode* CardSimpleNode::create(const CardSimpleData* data)
{
    auto ret = new (nothrow) CardSimpleNode();
    if(ret && ret->init(data)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CardSimpleNode::CardSimpleNode()
:_data(nullptr)
,_icon(nullptr)
,_callback(nullptr) {}

CardSimpleNode::~CardSimpleNode()
{
    removeFromParent();
}

bool CardSimpleNode::init(const CardSimpleData* data)
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(Size(Width, Height));
        
        static const auto file(CocosUtils::getResourcePath("icon_kapai.png"));
        auto button = ui::Button::create(file, file);
        button->addClickEventListener([this](Ref *pSender){
            if (_callback) {
                _callback();
            }
        });
        button->setPosition(Point(Width / 2, Height / 2));
        addChild(button);
        
        _icon = button;
        
        update(data);
        
        return true;
    }
    
    return false;
}

#pragma mark - public
void CardSimpleNode::update(const CardSimpleData* data)
{
    _data = data;
    
    // update info
    if (data) {
        
    } else {
        
    }
}

void CardSimpleNode::setCallback(const Callback& callback)
{
    _callback = callback;
}
