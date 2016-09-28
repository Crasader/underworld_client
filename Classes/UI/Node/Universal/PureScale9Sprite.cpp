//
//  PureScale9Sprite.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "PureScale9Sprite.h"
#include "CocosUtils.h"

using namespace std;

PureScale9Sprite* PureScale9Sprite::create(Type type)
{
    auto ret = new (nothrow) PureScale9Sprite();
    if (ret && ret->init(type)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

PureScale9Sprite::PureScale9Sprite()
:_type(Type::White) {}

PureScale9Sprite::~PureScale9Sprite() {}

bool PureScale9Sprite::init(Type type)
{
    auto file(getResourceFile(type));
    
    if (!file.empty() && ui::Scale9Sprite::initWithFile(file)) {
        _type = type;
        return true;
    }
    
    return false;
}

void PureScale9Sprite::setType(Type type)
{
    auto file(getResourceFile(type));
    if (!file.empty() && type != _type) {
        _type = type;
        auto size(getContentSize());
        setSpriteFrame(Sprite::create(file)->getSpriteFrame());
        setContentSize(size);
    }
}

string PureScale9Sprite::getResourceFile(Type type) const
{
    switch (type) {
        case Type::White:
            return CocosUtils::getResourcePath("ui_scaleable_white.png");
        case Type::Purple:
            return CocosUtils::getResourcePath("ui_scaleable_purple.png");
        case Type::BlueLight:
            return CocosUtils::getResourcePath("ui_scaleable_bluelight.png");
        case Type::BlueDeep:
            return CocosUtils::getResourcePath("ui_scaleable_bluedeep.png");
        case Type::Blue:
            return CocosUtils::getResourcePath("ui_scaleable_blue.png");
        case Type::Orange:
            return CocosUtils::getResourcePath("ui_scaleable_orange.png");
        default:
            return "";
    }
}
