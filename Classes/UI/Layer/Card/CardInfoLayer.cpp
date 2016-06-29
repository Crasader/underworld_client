//
//  CardInfoLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/6/29.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardInfoLayer.h"
#include "CocosUtils.h"
#include "CardInfoNode.h"

using namespace std;

CardInfoLayer* CardInfoLayer::create()
{
    CardInfoLayer *ret = new (nothrow) CardInfoLayer();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CardInfoLayer::CardInfoLayer()
:_observer(nullptr)
,_cardInfoNode(nullptr)
,_titleLabel(nullptr) {}

CardInfoLayer::~CardInfoLayer()
{
    removeAllChildren();
}

bool CardInfoLayer::init()
{
    if (Layer::init())
    {
        _cardInfoNode = CardInfoNode::create([this](const string& name, int cost) {
            if (_observer) {
                _observer->onCardInfoLayerUpgradeCard(name);
            }
        });
        
        static const float edgeX(8.0f);
        const auto& winSize = Director::getInstance()->getWinSize();
        const auto& cardSize(_cardInfoNode->getContentSize());
        const float width = cardSize.width;
        const Size size(width + edgeX * 2, winSize.height);
        auto bg = CocosUtils::createPureColorSprite(size, LAYER_MASK_COLOR);
        bg->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(bg);
        
        auto titleSprite = CocosUtils::createTitle("Name", TITLE_FONT_SIZE);
        _titleLabel = dynamic_cast<Label*>(titleSprite->getChildren().front());
        static const float offsetY(12.0f);
        const float titleHeight = titleSprite->getContentSize().height;
        titleSprite->setPosition(Point(size.width / 2, size.height - (titleHeight / 2 + offsetY)));
        bg->addChild(titleSprite);
        
        Sprite* line = Sprite::create("GameImages/public/ui_line.png");
        line->setScaleX(width / line->getContentSize().width);
        line->setPosition(titleSprite->getPosition() - Point(0, titleHeight / 2 + offsetY));
        bg->addChild(line);
        
        _cardInfoNode->setPosition(Point(edgeX + cardSize.width / 2, line->getPosition().y - cardSize.height / 2));
        bg->addChild(_cardInfoNode);
        
        CocosUtils::createExitButton(bg, [this](){
            removeFromParent();
            if (_observer) {
                _observer->onCardInfoLayerUpgradeCard(_name);
            }
        });
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(CardInfoLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(CardInfoLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool CardInfoLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void CardInfoLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}

void CardInfoLayer::registerObserver(CardInfoLayerObserver *observer)
{
    _observer = observer;
}

void CardInfoLayer::update(const string& name)
{
    _name = name;
    
    if (_cardInfoNode) {
        _cardInfoNode->update(name);
    }
    
    if (_titleLabel) {
        _titleLabel->setString(name);
    }
}
