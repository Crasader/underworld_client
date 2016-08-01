//
//  AbstractUILayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/2/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "AbstractUILayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"

using namespace std;

AbstractUILayer::AbstractUILayer()
:_background(nullptr)
,_closeButton(nullptr)
{
    
}

AbstractUILayer::~AbstractUILayer()
{
    removeAllChildren();
}

bool AbstractUILayer::init(const string& title)
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR))
    {
        const Size& winSize = Director::getInstance()->getWinSize();
        
        _background = CocosUtils::createPureColorSprite(Size(886, 640), LAYER_MASK_COLOR);
        _background->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(_background);
        
        Sprite* titleSprite = CocosUtils::createTitle(title, TITLE_FONT_SIZE);
        _background->addChild(titleSprite);
        
        Sprite* line = Sprite::create("GameImages/public/ui_line.png");
        line->setScaleX(870 / line->getContentSize().width);
        _background->addChild(line);
        
        _closeButton = dynamic_cast<Button*>(CocosUtils::createGrayExitButton(_background, [this](){ removeFromParent(); }));
        
        const Size& size = _background->getContentSize();
        
        const Size& titleSize = titleSprite->getContentSize();
        static const float offsetY(12.0f);
        titleSprite->setPosition(Point(size.width / 2, size.height - (titleSize.height / 2 + offsetY)));
        line->setPosition(titleSprite->getPosition() - Point(0, titleSize.height / 2 + offsetY));
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(AbstractUILayer::onTouchBegan, this);
        eventListener->onTouchMoved = CC_CALLBACK_2(AbstractUILayer::onTouchMoved, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(AbstractUILayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool AbstractUILayer::onTouchBegan(Touch *touch, Event *unused_event)
{
    return true;
}

void AbstractUILayer::onTouchMoved(Touch *touch, Event *unused_event)
{
    
}

void AbstractUILayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}
