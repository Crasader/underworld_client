//
//  PureNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "PureNode.h"
#include "CocosUtils.h"

using namespace std;

PureNode* PureNode::create(const Color4B& color, const Size& size)
{
    auto ret = new (nothrow) PureNode();
    if (ret && ret->init(color, size)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

PureNode* PureNode::createLine(const Size& size)
{
    return create(Color4B::BLACK, size);
}

PureNode::PureNode()
:_color(Color4B(0, 0, 0, 0))
,_sprite(nullptr) {}

PureNode::~PureNode() {}

bool PureNode::init(const Color4B& color, const Size& size)
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        
        static auto file(CocosUtils::getResourcePath("ui_pure_white.png"));
        if (!file.empty()) {
            auto sprite = Sprite::create(file);
            const auto& spriteSize(sprite->getContentSize());
            sprite->setScale(size.width / spriteSize.width, size.height / spriteSize.height);
            sprite->setPosition(Point(size.width / 2, size.height / 2));
            addChild(sprite);
            _sprite = sprite;
            
            setColor(color);
        }
        
        return true;
    }
    
    return false;
}

void PureNode::setColor(const Color4B& color)
{
    if (_color != color) {
        _color = color;
        if (_sprite) {
            _sprite->setColor(Color3B(color.r, color.g, color.b));
            _sprite->setOpacity(color.a);
        }
    }
}

void PureNode::setSize(const Size& size)
{
    if (_sprite) {
        const auto& spriteSize(_sprite->getContentSize());
        _sprite->setScale(size.width / spriteSize.width, size.height / spriteSize.height);
        setContentSize(size);
    }
}
