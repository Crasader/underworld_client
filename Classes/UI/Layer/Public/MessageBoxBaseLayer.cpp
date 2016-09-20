//
//  MessageBoxBaseLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MessageBoxBaseLayer.h"
#include "XButton.h"
#include "SmallBoard.h"
#include "CocosGlobal.h"
#include "LocalHelper.h"

using namespace std;

MessageBoxBaseLayer::MessageBoxBaseLayer()
:_board(nullptr)
,_confirmButton(nullptr)
,_confirmButtonPosition(Point::ZERO)
,_cancelButton(nullptr)
,_cancelButtonPosition(Point::ZERO) {}

MessageBoxBaseLayer::~MessageBoxBaseLayer()
{
    removeAllChildren();
}

bool MessageBoxBaseLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR))
    {
        static cocos2d::Size message_box_size(382.f, 247.f);
        static cocos2d::Size background_size(360.f, 176.f);
        static float margin_horizontal(10.f);
        static float margin_bottom(16.f);
        static float inner_margin_horizontal(10.f);
        static float inner_margin_bottom(6.f);
        
        const auto& winSize = Director::getInstance()->getWinSize();
        auto board = SmallBoard::create();
        board->setTitle("");
        board->setExitButtonVisible(false);
        board->setExitCallback([this]() {
            removeFromParent();
        });
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        _board = board;
        
        _confirmButton = XButton::create(XButton::BSize::Big, XButton::BType::Red);
        _confirmButton->setTitleText(LocalHelper::getString("hint_confirm"));
        _confirmButton->setTitleColor(Color3B::WHITE);
        board->addChild(_confirmButton);
        
        _cancelButton = XButton::create(XButton::BSize::Big, XButton::BType::Blue);
        _cancelButton->setTitleText(LocalHelper::getString("hint_cancel"));
        _cancelButton->setTitleColor(Color3B::WHITE);
        board->addChild(_cancelButton);
        
        const auto& sizeg = _confirmButton->getContentSize();
        const auto& sizeb = _cancelButton->getContentSize();
        _confirmButtonPosition = Point(margin_horizontal + inner_margin_horizontal + sizeg.width / 2, margin_bottom + inner_margin_bottom + sizeg.height / 2);
        _cancelButtonPosition = Point(message_box_size.width - margin_horizontal - inner_margin_horizontal - sizeb.width / 2, margin_bottom + inner_margin_bottom + sizeb.height / 2);
        _confirmButton->setPosition(_confirmButtonPosition);
        _cancelButton->setPosition(_cancelButtonPosition);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = [](Touch *pTouch, Event *pEvent) { return true; };
        eventListener->onTouchEnded = [](Touch *pTouch, Event *pEvent) {};
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}
