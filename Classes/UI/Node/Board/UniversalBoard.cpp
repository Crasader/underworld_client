//
//  UniversalBoard.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "UniversalBoard.h"
#include "CocosUtils.h"
#include "XButton.h"

using namespace std;

UniversalBoard* UniversalBoard::create(const string& file)
{
    auto ret = new (nothrow) UniversalBoard();
    if (ret && ret->init(file)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

UniversalBoard::UniversalBoard()
:_title(nullptr)
,_exitButton(nullptr)
,_exitCallback(nullptr) {}

UniversalBoard::~UniversalBoard()
{
    removeAllChildren();
}

bool UniversalBoard::init(const string& file)
{
    if (Sprite::init()) {
        setTexture(CocosUtils::getResourcePath(file));
        
        _exitButton = CocosUtils::createRedExitButton(this, [this]() {
            if (_exitCallback) {
                _exitCallback();
            } else if (_parent) {
                _parent->removeFromParent();
            } else {
                removeFromParent();
            }
        });
        
        const auto& size(getContentSize());
        auto title = CocosUtils::createLabel("Untitled", TITLE_FONT_SIZE);
        title->setAnchorPoint(Point::ANCHOR_MIDDLE);
        title->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
        addChild(title);
        _title = title;
        
        static const float offsetY(10);
        const float height(title->getContentSize().height);
        title->setPosition(Point(size.width / 2, size.height - (offsetY + height / 2)));
        
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
        _exitCallback = callback;
    }
}
