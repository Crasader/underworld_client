//
//  CardOpButton.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardOpButton.h"
#include "LocalHelper.h"

CardOpButton* CardOpButton::create(CardOpType opType)
{
    auto p = new (std::nothrow) CardOpButton();
    if(p && p->init(opType)) {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

CardOpButton::CardOpButton() {}

CardOpButton::~CardOpButton()
{
    removeAllChildren();
}

bool CardOpButton::init(CardOpType opType)
{
    XButton::BType type(XButton::BType::Blue);
    std::string title;
    switch (opType) {
        case CardOpType::Upgrade: {
            type = XButton::BType::Green;
            title = LocalHelper::getString("ui_deck_upgrade");
        }
            break;
        case CardOpType::Use: {
            type = XButton::BType::Green;
            title = LocalHelper::getString("ui_deck_use");
        }
            break;
        case CardOpType::Info: {
            title = LocalHelper::getString("ui_deck_info");
        }
            break;
        case CardOpType::Move: {
            type = XButton::BType::Green;
            title = LocalHelper::getString("ui_deck_move");
        }
            break;
        default:
            break;
    }
    
    if (XButton::init(XButton::BSize::Small, type)) {
        setTitleText(title);
        return true;
    }
    
    return false;
}
