//
//  BigBoard.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "BigBoard.h"
#include "PureScale9Sprite.h"

using namespace std;

BigBoard* BigBoard::create()
{
    auto ret = new (nothrow) BigBoard();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

BigBoard::BigBoard()
:_subNode(nullptr) {}

BigBoard::~BigBoard() {}

bool BigBoard::init()
{
    if (UniversalBoard::init("ui_background_2.png")) {
        const auto& size(getContentSize());
        static const Size subSize(579, 513);
        const float edge((size.width - subSize.width) / 2);
        
        auto subNode = PureScale9Sprite::create(PureScale9Sprite::Type::BlueLight);
        subNode->setContentSize(subSize);
        subNode->setPosition(Point(size.width / 2, subSize.height / 2 + edge));
        addChild(subNode);
        _subNode = subNode;
        
        return true;
    }
    
    return false;
}

Node* BigBoard::getSubNode() const
{
    return _subNode;
}
