//
//  MessageBoxBaseLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MessageBoxBaseLayer.h"
#include "XButton.h"
#include "CocosGlobal.h"
#include "LocalHelper.h"

using namespace std;

MessageBoxBaseLayer::MessageBoxBaseLayer()
:_background_1(nullptr)
,_background_2(nullptr)
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
        
        cocos2d::Rect rect(0, 0, 56, 56);
        cocos2d::Rect capInsetsRect(14, 12, 34, 32);
        _background_1 = cocos2d::ui::Scale9Sprite::create("GameImages/messagebox/background_1.png", rect, capInsetsRect);
        _background_1->setContentSize(message_box_size);
        _background_1->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(_background_1);
        
        
        rect.setRect(0, 0, 56, 56);
        capInsetsRect.setRect(10, 10, 36, 36);
        _background_2 = cocos2d::ui::Scale9Sprite::create("GameImages/messagebox/background_2.png", rect, capInsetsRect);
        _background_2->setContentSize(background_size);
        _background_2->setPosition(cocos2d::Vec2(margin_horizontal + background_size.width / 2, margin_bottom + background_size.height / 2));
        _background_1->addChild(_background_2);
        
        _confirmButton = XButton::create(XButton::BSize::Big, XButton::BType::Red);
        _confirmButton->setTitleText(LocalHelper::getString("hint_confirm"));
        _background_1->addChild(_confirmButton);
        
        _cancelButton = XButton::create(XButton::BSize::Big, XButton::BType::Blue);
        _cancelButton->setTitleText(LocalHelper::getString("hint_cancel"));
        _background_1->addChild(_cancelButton);
        
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
