//
//  UserOperationNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/7/27.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "UserOperationNode.h"
#include "CocosUtils.h"
#include "ChatUI.h"
#include "LocalHelper.h"
#include "UniversalButton.h"

using namespace std;
using namespace ui;

static const vector<string> buttonTitles = {
    LocalHelper::getString("ui_chat_user_detail"),
    LocalHelper::getString("ui_chat_user_makeFriend"),
    LocalHelper::getString("ui_chat_user_block"),
};

UserOperationNode* UserOperationNode::create(int uid)
{
    auto ret = new (nothrow) UserOperationNode();
    if (ret && ret->init(uid)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

UserOperationNode::UserOperationNode()
:_observer(nullptr)
,_bg(nullptr)
,_uid(0) {}

UserOperationNode::~UserOperationNode()
{
    removeAllChildren();
}

void UserOperationNode::registerObserver(UserOperationNodeObserver *observer)
{
    _observer = observer;
}

bool UserOperationNode::init(int uid)
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        
        static const Size s(164, 260);
        static const float cx(22);
        static const float cy(10);
        _bg = Scale9Sprite::create(ChatUI::getResourcePath("ui_kuang_4.png"), Rect(0, 0, s.width, s.height), Rect(cx, cy, s.width - (cx + cy), s.height - cy * 2));
        addChild(_bg);
        
        static const float edge(3);
        static const float space(5);
        Size bsize(Size::ZERO);
        Size size(Size::ZERO);
        const auto cnt(buttonTitles.size());
        for (int i = 0; i < cnt; ++i) {
            const auto& t(buttonTitles.at(i));
            auto button = UniversalButton::create(UniversalButton::BSize::Big, UniversalButton::BType::Blue, t);
            button->setCallback([this, i](Ref*) {
                onButtonClicked(i);
            });
            _bg->addChild(button);
            _buttons.push_back(button);
            
            if (0 == i) {
                bsize = button->getContentSize();
                size.width = edge * 2 + bsize.width;
                size.height = edge * 2 + (cnt - 1) * space + cnt * bsize.height;
                _bg->setContentSize(size);
            }
            
            button->setPosition(Point(size.width / 2, size.height - (edge + (i + 0.5) * bsize.height + (i - 1) * space)));
        }
        
        update(uid);
        
        return true;
    }
    
    return false;
}

void UserOperationNode::update(int uid)
{
    _uid = uid;
    adjust();
}

void UserOperationNode::adjust()
{
    
}

void UserOperationNode::onButtonClicked(int idx)
{
    
}
