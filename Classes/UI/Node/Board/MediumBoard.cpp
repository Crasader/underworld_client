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
#include "UniversalButton.h"

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
,_button(nullptr)
,_buttonCallback(nullptr) {}

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
        
        auto button = UniversalButton::create(UniversalButton::BSize::Big, UniversalButton::BType::Blue, "Untitled");
        button->setCallback([this](Ref*) {
            if (_buttonCallback) {
                _buttonCallback();
            }
        });
        button->setPosition(Point(size.width / 2, edge / 2));
        addChild(button);
        _button = button;
        
        return true;
    }
    
    return false;
}

void MediumBoard::setButtonVisible(bool visible)
{
    if (_button) {
        _button->setVisible(visible);
    }
}

void MediumBoard::setButtonTitle(const std::string& title)
{
    if (_button) {
        _button->setTitle(title);
    }
}

void MediumBoard::setButtonCallback(const Callback& callback)
{
    if (_button) {
        _buttonCallback = callback;
    }
}

Node* MediumBoard::getSubNode() const
{
    return _subNode;
}
