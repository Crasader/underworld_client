//
//  MessageBoxBaseLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MessageBoxBaseLayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"

using namespace std;

MessageBoxBaseLayer::MessageBoxBaseLayer()
:_background(nullptr)
,_confirmButton(nullptr)
,_confirmButtonPosition(Point::ZERO)
,_cancelButton(nullptr)
,_cancelButtonPosition(Point::ZERO)
{
    
}

MessageBoxBaseLayer::~MessageBoxBaseLayer()
{
    removeAllChildren();
}

bool MessageBoxBaseLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR))
    {
        const Size& winSize = Director::getInstance()->getWinSize();
        
        _background = Sprite::create("GameImages/messagebox/background.png");
        _background->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(_background);
        
        _confirmButton = Button::create("GameImages/public/ui_button_green.png");
        _background->addChild(_confirmButton);
        
        _cancelButton = Button::create("GameImages/public/ui_button.png");
        _background->addChild(_cancelButton);
        
        const Size& size = _background->getContentSize();
        const Size& sizeg = _confirmButton->getContentSize();
        const Size& sizeb = _cancelButton->getContentSize();
        static const float offsetY = 30.0f;
        _confirmButtonPosition = Point(size.width * 0.25f, sizeg.height / 2 + offsetY);
        _cancelButtonPosition = Point(size.width * 0.75f, sizeb.height / 2 + offsetY);
        _confirmButton->setPosition(_confirmButtonPosition);
        _cancelButton->setPosition(_cancelButtonPosition);
        
        _confirmLabel = CocosUtils::createLabel(LocalHelper::getString("hint_confirm"), BIG_FONT_SIZE, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
        _confirmLabel->setPosition(Point(sizeg.width / 2, sizeg.height * 0.55f));
        _confirmButton->addChild(_confirmLabel);
        
        _cancelLabel = CocosUtils::createLabel(LocalHelper::getString("hint_cancel"), BIG_FONT_SIZE, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
        _cancelLabel->setPosition(Point(sizeb.width / 2, sizeb.height * 0.55f));
        _cancelButton->addChild(_cancelLabel);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(MessageBoxBaseLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(MessageBoxBaseLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

void MessageBoxBaseLayer::onEnter()
{
    LayerColor::onEnter();
}

bool MessageBoxBaseLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
    return true;
}

void MessageBoxBaseLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}