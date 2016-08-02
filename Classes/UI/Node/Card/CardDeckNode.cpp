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

static const unsigned int row(2);
static const unsigned int column(6);
static const Vec2 space(5, 3);

CardDeckNode* CardDeckNode::create()
{
    auto ret = new (nothrow) CardDeckNode();
    if(ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CardDeckNode::CardDeckNode()
{}

CardDeckNode::~CardDeckNode()
{
    removeFromParent();
}

bool CardDeckNode::init()
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        const Size size(CardSimpleNode::Width * column + space.x * (column - 1), CardSimpleNode::Height * row + space.y * (row - 1));
        setContentSize(size);
        
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
    for (int i = 0; i < row; ++i) {
        bool loop(true);
        for (int j = 0; j < column; ++j) {
            const size_t index(i * column + j);
            if (vec.size() > index) {
                if (_nodes.size() > index) {
                    _nodes.at(index)->update(vec.at(index));
                } else {
                    auto node = CardSimpleNode::create(vec.at(index));
                    node->setPosition((w + space.x) * j + w / 2, (h + space.y) * (row - (i + 1)) + h / 2);
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
