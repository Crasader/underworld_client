//
//  XEditBox.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/8.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "XEditBox.h"
#include "CocosUtils.h"

XEditBox* XEditBox::create(const Size& size)
{
    auto p = new (std::nothrow) XEditBox();
    if(p && p->init(size)) {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

XEditBox::XEditBox() {}

XEditBox::~XEditBox()
{
    removeAllChildren();
}

bool XEditBox::init(const Size& size)
{
    auto sprite = ui::Scale9Sprite::create(CocosUtils::getResourcePath("ui_kuang_6.png"));
    
    if (ui::EditBox::initWithSizeAndBackgroundSprite(size, sprite)) {
        setMaxLength(size.width);
        setReturnType(ui::EditBox::KeyboardReturnType::SEND);
        setInputMode(ui::EditBox::InputMode::ANY);
        setFontColor(Color4B::BLACK);
        
        return true;
    }
    
    return false;
}
