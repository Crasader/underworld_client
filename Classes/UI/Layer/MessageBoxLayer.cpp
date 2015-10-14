//
//  MessageBoxLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MessageBoxLayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "SoundManager.h"

using namespace std;

MessageBoxLayer::MessageBoxLayer()
:_type(kMessageBoxOnlyYes)
,_autoHide(true)
,_confirmCallback(nullptr)
,_cancelCallback(nullptr)
,_messageLabel(nullptr)
,_messageLabelPosition(Point::ZERO)
{
    
}

MessageBoxLayer::~MessageBoxLayer()
{
    removeAllChildren();
}

static MessageBoxLayer* s_pSharedLayer = nullptr;

MessageBoxLayer * MessageBoxLayer::getInstance()
{
    if (s_pSharedLayer == nullptr)
    {
        s_pSharedLayer = new (nothrow) MessageBoxLayer();
        
        if (s_pSharedLayer)
        {
            s_pSharedLayer->init();
        }
    }
    
    return s_pSharedLayer;
}

void MessageBoxLayer::purge()
{
    if (s_pSharedLayer)
    {
        s_pSharedLayer->removeFromParent();
        
        CC_SAFE_DELETE(s_pSharedLayer);
        s_pSharedLayer = nullptr;
    }
}

bool MessageBoxLayer::init()
{
    if (MessageBoxBaseLayer::init())
    {
        static const Size dimensions(440, 120);
        _messageLabel = CocosUtils::createLabel("", MESSAGEBOX_DEFAULT_FONT_SIZE, dimensions, TextHAlignment::CENTER, TextVAlignment::CENTER);
        _background->addChild(_messageLabel);
        
        const Size& size = _background->getContentSize();
        _messageLabelPosition = Point(size.width / 2, 180);
        _messageLabel->setPosition(_messageLabelPosition);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(MessageBoxLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(MessageBoxLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

void MessageBoxLayer::show(const std::string& message, int fontZie)
{
    resetMessageLabel(fontZie);
    _type = kMessageBoxNoYes;
    _autoHide = true;
    _confirmCallback = nullptr;
    _cancelCallback = nullptr;
    _confirmButton->setVisible(false);
    _cancelButton->setVisible(false);
    
    if (getParent() != nullptr)
    {
        hide();
    }
    
    if (_messageLabel)
    {
        const Size& size = _background->getContentSize();
        _messageLabel->setPosition(Point(size.width / 2, size.height / 2));
        _messageLabel->setString(message);
    }
    
    Director::getInstance()->getRunningScene()->addChild(this);
    SoundManager::getInstance()->playMessageHintSound();
}

void MessageBoxLayer::show(const std::string& message, MessageBoxType type, const Button::ccWidgetClickCallback& confirmCallback, const Button::ccWidgetClickCallback& cancelCallback, bool autoHide, const std::string& confirmString, const std::string& cancelString)
{
    resetMessageLabel(MESSAGEBOX_DEFAULT_FONT_SIZE);
    
    _type = type;
    _autoHide = autoHide;
    
    const Button::ccWidgetClickCallback defaultCallback = [this](Ref *){ hide(); };
    _confirmCallback = confirmCallback ? confirmCallback : defaultCallback;
    _cancelCallback = cancelCallback ? cancelCallback : defaultCallback;
    
    if (getParent() != nullptr)
    {
        hide();
    }
    
    _confirmLabel->setString(confirmString);
    _cancelLabel->setString(cancelString);
    
    if (_messageLabel)
    {
        _messageLabel->setPosition(_messageLabelPosition);
        _messageLabel->setString(message);
    }
    
    if (type == kMessageBoxOnlyYes)
    {
        _cancelButton->setVisible(false);
        _confirmButton->setVisible(true);
        _confirmButton->setPosition(_confirmButtonPosition.getMidpoint(_cancelButtonPosition));
    }
    else if (type == kMessageBoxYesNo)
    {
        _cancelButton->setVisible(true);
        _cancelButton->setPosition(_cancelButtonPosition);
        _confirmButton->setVisible(true);
        _confirmButton->setPosition(_confirmButtonPosition);
    }
    else if (type == kMessageBoxNoYes)
    {
        _cancelButton->setVisible(true);
        _cancelButton->setPosition(_confirmButtonPosition);
        _confirmButton->setVisible(true);
        _confirmButton->setPosition(_cancelButtonPosition);
    }
    
    _confirmButton->addClickEventListener([this](Ref *pSender) {
        SoundManager::getInstance()->playButtonSound();
        _confirmCallback(pSender);
    });
    _cancelButton->addClickEventListener([this](Ref *pSender) {
        SoundManager::getInstance()->playButtonCancelSound();
        _cancelCallback(pSender);
    });
    
    Director::getInstance()->getRunningScene()->addChild(this);
    SoundManager::getInstance()->playMessageHintSound();
}

void MessageBoxLayer::hide()
{
    removeFromParent();
}

void MessageBoxLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    Point point = convertToWorldSpace(touch->getLocation());
    Rect rs = _background->getBoundingBox();
    rs.origin = convertToWorldSpace(rs.origin);
    if (_confirmCallback == nullptr)
    {
        hide();
    }
    else if (_autoHide && false == rs.containsPoint(point))
    {
        if (_type == kMessageBoxOnlyYes)
        {
            SoundManager::getInstance()->playButtonSound();
            _confirmCallback(nullptr);
        }
        else
        {
            SoundManager::getInstance()->playButtonCancelSound();
            _cancelCallback(nullptr);
        }
    }
}

void MessageBoxLayer::resetMessageLabel(int fontSize)
{
    if (!_messageLabel || fontSize == _messageLabel->getTTFConfig().fontSize) return;
    
    TTFConfig ttfConfig(_messageLabel->getTTFConfig());
    ttfConfig.fontSize = fontSize;
    _messageLabel->setTTFConfig(ttfConfig);
}