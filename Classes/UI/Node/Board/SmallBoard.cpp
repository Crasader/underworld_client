//
//  SmallBoard.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "SmallBoard.h"
#include "PureScale9Sprite.h"

using namespace std;

SmallBoard* SmallBoard::create()
{
    auto ret = new (nothrow) SmallBoard();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

SmallBoard::SmallBoard()
:_subNode(nullptr) {}

SmallBoard::~SmallBoard() {}

bool SmallBoard::init()
{
    if (UniversalBoard::init("ui_background_4.png")) {
        const auto& size(getContentSize());
        static const Size subSize(359, 174);
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

Node* SmallBoard::getSubNode() const
{
    return _subNode;
}
