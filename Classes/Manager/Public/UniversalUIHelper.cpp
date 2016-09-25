//
//  UniversalUIHelper.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "UniversalUIHelper.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "DataManager.h"
#include "CardData.h"
#include "CardProperty.h"
#include "CardInfoLayer.h"
#include "SpellInfoLayer.h"

using namespace std;

#pragma mark - CardInfoUIObserver
class CardInfoUIObserver
: public CardInfoLayerObserver
, public SpellInfoLayerObserver {
public:
    CardInfoUIObserver();
    virtual ~CardInfoUIObserver();
    
    // CardInfoLayerObserver
    virtual void onCardInfoLayerReturn(Node* pSender) override;
    virtual void onCardInfoLayerExit(Node* pSender) override;
    
    // SpellInfoLayerObserver
    virtual void onSpellInfoLayerExit(Node* pSender) override;
    virtual void onSpellInfoLayerUpgrade(Node* pSender, const AbstractData* data) override;
};

CardInfoUIObserver::CardInfoUIObserver() {}

CardInfoUIObserver::~CardInfoUIObserver() {}

#pragma mark - CardInfoLayerObserver
void CardInfoUIObserver::onCardInfoLayerReturn(Node* pSender)
{
    if (pSender) {
        pSender->removeFromParent();
    }
}

void CardInfoUIObserver::onCardInfoLayerExit(Node* pSender)
{
    if (pSender) {
        auto parent(pSender->getParent());
        if (parent) {
            parent->removeAllChildren();
            parent->removeFromParent();
        }
    }
}

#pragma mark - SpellInfoLayerObserver
void CardInfoUIObserver::onSpellInfoLayerExit(Node* pSender)
{
    if (pSender) {
        pSender->removeFromParent();
    }
}

void CardInfoUIObserver::onSpellInfoLayerUpgrade(Node* pSender, const AbstractData* data)
{
    if (pSender) {
        pSender->removeFromParent();
    }
}

#pragma mark - UniversalUIHelper
static UniversalUIHelper* s_pInstance(nullptr);
UniversalUIHelper* UniversalUIHelper::getInstance()
{
    if (!s_pInstance) {
        s_pInstance = new (nothrow) UniversalUIHelper();
    }
    
    return s_pInstance;
}

void UniversalUIHelper::purge()
{
    if (s_pInstance) {
        CC_SAFE_DELETE(s_pInstance);
    }
}

UniversalUIHelper::UniversalUIHelper()
:_messageStartPoint(Point::ZERO)
,_messageAction(nullptr)
,_cardInfoUIObserver(nullptr)
{
    _cardInfoUIObserver = new (nothrow) CardInfoUIObserver();
}

UniversalUIHelper::~UniversalUIHelper()
{
    CC_SAFE_RELEASE(_messageAction);
    CC_SAFE_DELETE(_cardInfoUIObserver);
}

Node* UniversalUIHelper::showMessage(const std::string& message)
{
    if (message.empty()) {
        return nullptr;
    }
    
    auto director(Director::getInstance());
    
    if (Point::ZERO == _messageStartPoint) {
        const auto& winSize(director->getWinSize());
        _messageStartPoint = Point(winSize.width / 2, winSize.height * 0.7f);
    }
    
    if (!_messageAction) {
        auto spawn = Spawn::create(ScaleTo::create(0.1f, 1.0f),
                                   MoveBy::create(1.5f, Point(0, 20)),
                                   nullptr);
        auto action = Sequence::create(spawn,
                                       DelayTime::create(0.2f),
                                       FadeOut::create(1.5f),
                                       CallFuncN::create([this](Node *) {
            if (!_labels.empty()) {
                auto label(_labels.front());
                label->removeFromParent();
                _labels.pop();
            }
        }),nullptr);
        CC_SAFE_RETAIN(action);
        _messageAction = action;
    }
    
    Label* label(nullptr);
    do {
        CC_BREAK_IF(_labels.empty());
        auto tail(_labels.back());
        CC_BREAK_IF(!tail || tail->getString() != message);
        label = tail;
    } while (false);
    
    if (!label) {
        label = CocosUtils::createLabel(message, DEFAULT_FONT_SIZE);
        director->getRunningScene()->addChild(label);
        _labels.push(label);
    }
    
    if (label) {
        static const int actionTag(2016924);
        label->stopActionByTag(actionTag);
        label->setScale(0);
        label->setPosition(_messageStartPoint);
        auto action(_messageAction->clone());
        action->setTag(actionTag);
        label->runAction(action);
    }
    
    return label;
}

Node* UniversalUIHelper::showCardInfoLayer(Node* parent, int cardId, const AbstractData* data)
{
    Node* ret(nullptr);
    do {
        CC_BREAK_IF(!parent || cardId <= 0);
        auto property(DataManager::getInstance()->getProperty(cardId));
        CC_BREAK_IF(!property);
        auto cardProperty(dynamic_cast<const CardProperty*>(property));
        CC_BREAK_IF(!cardProperty);
        if (UnderWorld::Core::HMMCardClass::kHMMCardClass_Spell == cardProperty->getCardClass()) {
            auto layer = SpellInfoLayer::create(cardId, data);
            layer->registerObserver(_cardInfoUIObserver);
            parent->addChild(layer);
            ret = layer;
        } else {
            auto layer = CardInfoLayer::create(cardId, data);
            layer->registerObserver(_cardInfoUIObserver);
            parent->addChild(layer);
            ret = layer;
        }
    } while (false);
    
    return ret;
}
