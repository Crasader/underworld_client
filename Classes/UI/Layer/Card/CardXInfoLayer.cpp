//
//  CardXInfoLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/6/29.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardXInfoLayer.h"
#include "CocosUtils.h"
#include "CardInfoNode.h"
#include "TalentInfoNode.h"

using namespace std;

CardXInfoLayer* CardXInfoLayer::create()
{
    CardXInfoLayer *ret = new (nothrow) CardXInfoLayer();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CardXInfoLayer::CardXInfoLayer()
:_observer(nullptr)
,_cardInfoNode(nullptr)
,_talentInfoNode(nullptr)
,_titleLabel(nullptr) {}

CardXInfoLayer::~CardXInfoLayer()
{
    removeAllChildren();
}

bool CardXInfoLayer::init()
{
    if (Layer::init())
    {
        _cardInfoNode = CardInfoNode::create([this](const string& name, int cost) {
            if (_observer) {
                _observer->onCardXInfoLayerUpgradeCard(name);
            }
        });
        
        _talentInfoNode = TalentInfoNode::create([this](const string& name, int cost) {
            if (_observer) {
                _observer->onCardXInfoLayerUpgradeTalent(name);
            }
        });
        
        static const float edgeX(8.0f);
        const auto& winSize = Director::getInstance()->getWinSize();
        const auto& cardSize(_cardInfoNode->getContentSize());
        const auto& talentSize(_talentInfoNode->getContentSize());
        const float width = cardSize.width + talentSize.width;
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
        
        _talentInfoNode->setPosition(Point(size.width - (edgeX + talentSize.width / 2), line->getPosition().y - talentSize.height / 2));
        bg->addChild(_talentInfoNode);
        
        CocosUtils::createExitButton(bg, [this](){
            removeFromParent();
            if (_observer) {
                _observer->onCardXInfoLayerUpgradeCard(_name);
            }
        });
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(CardXInfoLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(CardXInfoLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool CardXInfoLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void CardXInfoLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}

void CardXInfoLayer::registerObserver(CardXInfoLayerObserver *observer)
{
    _observer = observer;
}

void CardXInfoLayer::update(const string& name)
{
    _name = name;
    
    if (_cardInfoNode) {
        _cardInfoNode->update(name);
    }
    
    if (_talentInfoNode) {
        _talentInfoNode->update(name);
    }
    
    if (_titleLabel) {
        _titleLabel->setString(name);
    }
}