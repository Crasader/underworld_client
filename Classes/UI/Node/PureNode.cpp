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

PureNode* PureNode::create(Type type, const Size& size)
{
    auto ret = new (nothrow) PureNode();
    if (ret && ret->init(type, size)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

PureNode::PureNode()
:_type(Type::White)
,_sprite(nullptr) {}

PureNode::~PureNode() {}

bool PureNode::init(Type type, const Size& size)
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        
        auto file(getResourceFile(type));
        if (!file.empty()) {
            _type = type;
            
            auto sprite = Sprite::create(file);
            const auto& spriteSize(sprite->getContentSize());
            sprite->setScale(size.width / spriteSize.width, size.height / spriteSize.height);
            sprite->setPosition(Point(size.width / 2, size.height / 2));
            addChild(sprite);
            
            _sprite = sprite;
        }
        
        return true;
    }
    
    return false;
}

void PureNode::setType(Type type)
{
    if (type != _type) {
        _type = type;
        
        auto file(getResourceFile(type));
        if (!file.empty() && _sprite) {
            _sprite->setTexture(getResourceFile(type));
        }
    }
}

void PureNode::setSize(const Size& size)
{
    if (_sprite) {
        const auto& spriteSize(_sprite->getContentSize());
        _sprite->setScale(size.width / spriteSize.width, size.height / spriteSize.height);
    }
}

string PureNode::getResourceFile(Type type) const
{
    switch (type) {
        case Type::White:
            return CocosUtils::getResourcePath("ui_pure_white.png");
        case Type::Gray:
            return CocosUtils::getResourcePath("ui_pure_gray.png");
        case Type::Black:
            return CocosUtils::getResourcePath("ui_pure_black.png");
        default:
            return "";
    }
}
