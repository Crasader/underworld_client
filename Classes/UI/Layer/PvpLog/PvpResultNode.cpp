//
//  PvpResultNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "PvpResultNode.h"
#include "CocosUtils.h"
#include "PvpLogUI.h"
#include "LocalHelper.h"

using namespace std;

PvpResultNode* PvpResultNode::create()
{
    PvpResultNode *p = new (nothrow) PvpResultNode();
    if(p && p->init()) {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

PvpResultNode::PvpResultNode()
:_bg(nullptr)
,_result(nullptr) {}

PvpResultNode::~PvpResultNode()
{
    removeFromParent();
}

bool PvpResultNode::init()
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        
        auto bg = Sprite::create(PvpLogUI::getResourcePath("ui_tiao_17.png"));
        addChild(bg);
        _bg = bg;
        
        auto result = CocosUtils::createLabel("", SMALL_FONT_SIZE, DEFAULT_NUMBER_FONT, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
        result->setAnchorPoint(Point::ANCHOR_MIDDLE);
        result->setTextColor(Color4B::BLACK);
        bg->addChild(result);
        _result = result;
        
        const auto& size(bg->getContentSize());
        result->setPosition(size.width / 2, size.height / 2);
        setContentSize(size);
        bg->setPosition(size.width / 2, size.height / 2);
        
        return true;
    }
    
    return false;
}

void PvpResultNode::setResult(bool win)
{
    if (_bg) {
        const string& file(PvpLogUI::getResourcePath(win ? "ui_tiao_17.png" : "ui_tiao_18.png"));
        _bg->setTexture(file);
    }
    
    if (_result) {
        _result->setString(win ? LocalHelper::getString("ui_log_win") : LocalHelper::getString("ui_log_lose"));
    }
}
