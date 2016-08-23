//
//  UniversalBoard.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "UniversalBoard.h"
#include "CocosUtils.h"

using namespace std;

UniversalBoard* UniversalBoard::create()
{
    auto ret = new (nothrow) UniversalBoard();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

UniversalBoard::UniversalBoard()
:_subNode(nullptr)
,_title(nullptr)
,_exitButton(nullptr)
,_callback(nullptr) {}

UniversalBoard::~UniversalBoard()
{
    removeAllChildren();
}

bool UniversalBoard::init()
{
    if (Sprite::init()) {
        setTexture(CocosUtils::getResourcePath("ui_background_5.png"));
        
        const auto& size(getContentSize());
        static const Size subSize(992, 513);
        auto subNode = CocosUtils::createSubBackground(subSize);
        addChild(subNode);
        const float edge((size.width - subSize.width) / 2);
        subNode->setPosition(Point(size.width / 2, subSize.height / 2 + edge));
        _subNode = subNode;
        
        _exitButton = CocosUtils::createRedExitButton(this, [this]() {
            if (_callback) {
                _callback();
            } else if (_parent) {
                _parent->removeFromParent();
            } else {
                removeFromParent();
            }
        });
        
        auto title = CocosUtils::createLabel("Untitled", TITLE_FONT_SIZE);
        title->setAnchorPoint(Point::ANCHOR_MIDDLE);
        title->setPosition(Point(size.width / 2, (size.height + subSize.height + edge) / 2));
        addChild(title);
        _title = title;
        
        return true;
    }
    
    return false;
}

void UniversalBoard::setTitle(const string& title)
{
    if (_title) {
        _title->setString(title);
    }
}

void UniversalBoard::setExitCallback(const Callback& callback)
{
    if (_exitButton) {
        _callback = callback;
    }
}

Node* UniversalBoard::getSubNode() const
{
    return _subNode;
}
