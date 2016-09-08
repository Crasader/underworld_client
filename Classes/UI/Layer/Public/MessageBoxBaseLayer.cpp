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
:_background(nullptr)
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
        const auto& winSize = Director::getInstance()->getWinSize();
        
        _background = Sprite::create("GameImages/messagebox/background.png");
        _background->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(_background);
        
        _confirmButton = XButton::create(XButton::BSize::Big, XButton::BType::Green);
        _confirmButton->setTitleText(LocalHelper::getString("hint_confirm"));
        _background->addChild(_confirmButton);
        
        _cancelButton = XButton::create(XButton::BSize::Big, XButton::BType::Blue);
        _cancelButton->setTitleText(LocalHelper::getString("hint_cancel"));
        _background->addChild(_cancelButton);
        
        const auto& size = _background->getContentSize();
        const auto& sizeg = _confirmButton->getContentSize();
        const auto& sizeb = _cancelButton->getContentSize();
        static const float offsetY = 30.0f;
        _confirmButtonPosition = Point(size.width * 0.25f, sizeg.height / 2 + offsetY);
        _cancelButtonPosition = Point(size.width * 0.75f, sizeb.height / 2 + offsetY);
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
