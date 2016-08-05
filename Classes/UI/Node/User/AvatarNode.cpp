//
//  AvatarNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "AvatarNode.h"
#include "CocosUtils.h"

using namespace std;

AvatarNode* AvatarNode::create()
{
    auto ret = new (nothrow) AvatarNode();
    if(ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

AvatarNode::AvatarNode()
:_avatar(nullptr)
,_callback(nullptr)
,_touchInvalid(false) {}

AvatarNode::~AvatarNode()
{
    removeFromParent();
}

bool AvatarNode::init()
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(Size(Width, Height));
        
        static const auto file(CocosUtils::getResourcePath("icon_touxiang_1.png"));
        auto avatar = ui::Button::create(file, file);
        avatar->addTouchEventListener([this](Ref *pSender, ui::Widget::TouchEventType type) {
            auto widget = dynamic_cast<ui::Widget*>(pSender);
            if (type == ui::Widget::TouchEventType::BEGAN) {
                _touchInvalid = false;
            } else if (type == ui::Widget::TouchEventType::MOVED) {
                if (!_touchInvalid) {
                    const auto& mp(widget->getTouchMovePosition());
                    const auto& bp(widget->getTouchBeganPosition());
                    static const float offset(40);
                    if (abs(mp.x - bp.x) >= offset || abs(mp.y - bp.y) >= offset) {
                        _touchInvalid = true;
                    }
                }
            } else if (type == ui::Widget::TouchEventType::ENDED) {
                if (!_touchInvalid && _callback) {
                    _callback();
                }
            }
        });
        avatar->setSwallowTouches(false);
        avatar->setPosition(Point(Width / 2, Height / 2));
        addChild(avatar);
        
        _avatar = avatar;
        
        return true;
    }
    
    return false;
}

void AvatarNode::setAvatar(int idx)
{
    if (_avatar) {
        static const auto file(CocosUtils::getResourcePath("icon_touxiang_1.png"));
        _avatar->loadTextures(file, file);
    }
}

void AvatarNode::setCallback(const Callback& callback)
{
    _callback = callback;
}
