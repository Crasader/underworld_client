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
    AvatarNode *p = new (nothrow) AvatarNode();
    if(p && p->init()) {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

AvatarNode::AvatarNode()
:_avatar(nullptr)
,_callback(nullptr) {}

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
        avatar->addClickEventListener([this](Ref *pSender){
            if (_callback) {
                _callback();
            }
        });
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
        if (_avatar) {
            _avatar->loadTextures(file, file);
        }
    }
}


void AvatarNode::setCallback(const Callback& callback)
{
    _callback = callback;
}
