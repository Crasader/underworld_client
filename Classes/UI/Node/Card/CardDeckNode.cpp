//
//  CardDeckNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardDeckNode.h"
#include "CardSimpleNode.h"
#include "CardSimpleData.h"

using namespace std;

CardDeckNode* CardDeckNode::create(const Size& size, size_t column, size_t row)
{
    auto ret = new (nothrow) CardDeckNode();
    if(ret && ret->init(size, column, row)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CardDeckNode::CardDeckNode()
:_column(1)
,_row(1)
,_space(Vec2(0,0)) {}

CardDeckNode::~CardDeckNode()
{
    removeFromParent();
}

bool CardDeckNode::init(const Size& size, size_t column, size_t row)
{
    if (Node::init()) {
        _column = column;
        _row = row;
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        
        if (column > 1) {
            _space.x = MAX(0, (size.width - column * CardSimpleNode::Width) / (column - 1));
        }
        
        if (row > 1) {
            _space.y = MAX(0, (size.height - row * CardSimpleNode::Height) / (row - 1));
        }
        
        return true;
    }
    
    return false;
}

void CardDeckNode::update(const vector<CardSimpleData*>& vec)
{
    // remove redundant nodes
    if (vec.size() < _nodes.size()) {
        auto i = vec.size();
        for (; i < _nodes.size(); ++i) {
            _nodes.at(i)->removeFromParent();
        }
        _nodes.erase(begin(_nodes) + i, end(_nodes));
    }
    
    static const float w(CardSimpleNode::Width);
    static const float h(CardSimpleNode::Height);
    for (int i = 0; i < _row; ++i) {
        bool loop(true);
        for (int j = 0; j < _column; ++j) {
            const size_t index(i * _column + j);
            if (vec.size() > index) {
                if (_nodes.size() > index) {
                    _nodes.at(index)->update(vec.at(index));
                } else {
                    auto node = CardSimpleNode::create(vec.at(index));
                    addChild(node);
                    node->setPosition((w + _space.x) * j + w / 2, (h + _space.y) * (_row - (i + 1)) + h / 2);
                    _nodes.push_back(node);
                }
            } else {
                loop = false;
                break;
            }
        }
        
        if (!loop) {
            break;
        }
    }
}
