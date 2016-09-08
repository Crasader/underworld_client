//
//  MediumBoard.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "MediumBoard.h"
#include "CocosUtils.h"
#include "PureScale9Sprite.h"
#include "XButton.h"

using namespace std;

MediumBoard* MediumBoard::create()
{
    auto ret = new (nothrow) MediumBoard();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

MediumBoard::MediumBoard()
:_subNode(nullptr)
,_midBottomChild(nullptr)
,_midBottomPoint(Point::ZERO) {}

MediumBoard::~MediumBoard() {}

bool MediumBoard::init()
{
    if (UniversalBoard::init("ui_background_3.png")) {
        const auto& size(getContentSize());
        static const Size subSize(579, 202);
        const float edge((size.height - subSize.height) / 2);
        
        auto subNode = PureScale9Sprite::create(PureScale9Sprite::Type::BlueLight);
        subNode->setContentSize(subSize);
        subNode->setPosition(Point(size.width / 2, subSize.height / 2 + edge));
        addChild(subNode);
        _subNode = subNode;
        
        _midBottomPoint = Point(size.width / 2, edge / 2);
        
        return true;
    }
    
    return false;
}

Node* MediumBoard::getSubNode() const
{
    return _subNode;
}

void MediumBoard::addChildToMidBottom(Node *child)
{
    if (_midBottomChild) {
        _midBottomChild->removeFromParent();
    }
    
    _midBottomChild = child;
    
    if (child) {
        CC_SAFE_RETAIN(child);
        child->removeFromParent();
        child->setPosition(_midBottomPoint);
        addChild(child);
        CC_SAFE_RELEASE(child);
    }
}
