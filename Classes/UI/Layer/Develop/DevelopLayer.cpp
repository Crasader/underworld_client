//
//  DevelopLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "DevelopLayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "Board.h"
#include "JuniorCard.h"
#include "CardData.h"
#include "CardProperty.h"

using namespace std;
using namespace ui;

DevelopLayer* DevelopLayer::create()
{
    auto ret = new (nothrow) DevelopLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

DevelopLayer::DevelopLayer()
:_observer(nullptr)
,_cardPreview(nullptr)
,_featureType(DeckManager::FeatureType::Develop) {}

DevelopLayer::~DevelopLayer()
{
    removeAllChildren();
}

#pragma mark - public
void DevelopLayer::registerObserver(DevelopLayerObserver *observer)
{
    _observer = observer;
}

#pragma mark - LayerColor
bool DevelopLayer::init()
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        auto board = Board::create(1);
        board->setTitle(LocalHelper::getString("ui_develop_title"));
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        
        if (!_cardPreview && board) {
            _cardPreview = new (nothrow) CardPreview(_featureType, board->getSubNode(0), this);
        }
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(DevelopLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(DevelopLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool DevelopLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void DevelopLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

#pragma mark - BaseCardObserver
void DevelopLayer::onBaseCardClicked(BaseCard* pSender)
{
    if (_cardPreview) {
        auto card(dynamic_cast<JuniorCard*>(pSender));
        if (card) {
            if (card->canUpgrade()) {
                _cardPreview->showOpNode(pSender, {CardOpType::Upgrade});
            } else {
                _cardPreview->showOpNode(pSender, {CardOpType::Info});
            }
        }
    }
}

#pragma mark - CardPreviewObserver
BaseCard* DevelopLayer::onCardPreviewCreateCard(int cardId)
{
    auto card = JuniorCard::create();
    card->registerObserver(this);
    card->update(cardId, DeckManager::getInstance()->getCardData(cardId));
    return card;
}

void DevelopLayer::onCardPreviewClickedOpButton(CardOpType type, const AbstractData* data)
{
    if (CardOpType::Upgrade == type || CardOpType::Info == type) {
        DeckManager::getInstance()->getCardDetail(data->getId(), [this](const CardData* data) {
            if (UnderWorld::Core::HMMCardClass::kHMMCardClass_Spell == dynamic_cast<const CardProperty*>(data->getProperty())->getCardClass()) {
                auto layer = SpellInfoLayer::create(data->getId(), data);
                layer->registerObserver(this);
                addChild(layer);
            } else {
                auto layer = CardInfoLayer::create(data->getId(), nullptr);
                layer->registerObserver(this);
                addChild(layer);
            }
        });
    }
}

#pragma mark - CardInfoLayerObserver
void DevelopLayer::onCardInfoLayerReturn(Node* pSender)
{
    if (pSender) {
        pSender->removeFromParent();
    }
}

void DevelopLayer::onCardInfoLayerExit(Node* pSender)
{
    if (pSender) {
        pSender->removeFromParent();
    }
    
    removeFromParent();
}

#pragma mark - SpellInfoLayerObserver
void DevelopLayer::onSpellInfoLayerExit(Node* pSender)
{
    if (pSender) {
        pSender->removeFromParent();
    }
}

void DevelopLayer::onSpellInfoLayerUpgrade(Node* pSender, const AbstractData* data)
{
    if (pSender) {
        pSender->removeFromParent();
    }
}
