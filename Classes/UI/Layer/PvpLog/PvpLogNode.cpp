//
//  PvpLogNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "PvpLogNode.h"
#include "CocosUtils.h"
#include "PvpLogData.h"
#include "UserSimpleData.h"
#include "CardSimpleData.h"
#include "PvpLogUI.h"

using namespace std;

PvpLogNode* PvpLogNode::create(const PvpLogData* data, bool expand)
{
    auto ret = new (nothrow) PvpLogNode();
    if (ret && ret->init(data, expand)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

PvpLogNode::PvpLogNode()
:_observer(nullptr)
,_data(nullptr) {}

PvpLogNode::~PvpLogNode()
{
    removeAllChildren();
}

void PvpLogNode::registerObserver(PvpLogNodeObserver *observer)
{
    _observer = observer;
}

bool PvpLogNode::init(const PvpLogData* data, bool expand)
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        
        string file;
        if (true) {
            file.assign(PvpLogUI::getResourcePath("ui_tiao_10.png"));
        } else {
            file.assign(PvpLogUI::getResourcePath("ui_tiao_9.png"));
        }
        
        static const Size size(975, 131);
        static const float offset(6);
        static const Rect capInsets(offset, offset, size.width - offset * 2, size.height - offset * 2);
        _background = ui::Scale9Sprite::create(file, Rect(0, 0, size.width, size.height), capInsets);
        _background->setAnchorPoint(Point::ANCHOR_MIDDLE);
        addChild(_background);
        
        update(data, expand);
        
        return true;
    }
    
    return false;
}

void PvpLogNode::update(const PvpLogData* data, bool expand)
{
    _data = data;
    _isExpanded = expand;
    adjust();
}

void PvpLogNode::show()
{
    _isExpanded = !_isExpanded;
    adjust();
}

void PvpLogNode::adjust()
{
    Size size(Width, 0);
    if (_isExpanded) {
        size.height = ExpandedHeight;
    } else {
        size.height = FoldedHeight;
    }
    _background->setContentSize(size);
    setContentSize(size);
    _background->setPosition(Point(size.width / 2, size.height / 2));
}

Node* PvpLogNode::createUserNode(bool isHome, const UserSimpleData* data)
{
    Node* node = Node::create();
    return node;
}

Node* PvpLogNode::createTowerNode(bool isHome, int percentage)
{
    Node* node = Node::create();
    return node;
}

Node* PvpLogNode::createCardsNode(const std::vector<CardSimpleData*>& data)
{
    Node* node = Node::create();
    return node;
}
