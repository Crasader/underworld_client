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
#include "DevelopUI.h"
#include "LocalHelper.h"
#include "Board.h"
#include "CardData.h"

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

#pragma mark - DevelopCardObserver
void DevelopLayer::onDevelopCardClicked(DevelopCard* pSender, bool canUpgrade)
{
    if (_cardPreview) {
        if (canUpgrade) {
            _cardPreview->showOpNode(pSender, {DeckCardOpType::Upgrade});
        } else {
            _cardPreview->showOpNode(pSender, {DeckCardOpType::Info});
        }
    }
}

#pragma mark - CardPreviewObserver
AbstractCard* DevelopLayer::onCardPreviewCreateCard(int cardId)
{
    auto card = DevelopCard::create(DeckManager::getInstance()->getCardData(cardId));
    card->registerObserver(this);
    return card;
}

void DevelopLayer::onCardPreviewClickedOpButton(DeckCardOpType type, const AbstractData* data)
{
    if (DeckCardOpType::Upgrade == type || DeckCardOpType::Info == type) {
        DeckManager::getInstance()->getCardDetail(data->getId(), [this](const CardData* data) {
            if (UnderWorld::Core::HMMCardClass::kHMMCardClass_Spell == data->getCardClass()) {
                auto layer = SpellInfoLayer::create(data);
                layer->registerObserver(this);
                addChild(layer);
            } else {
                auto layer = CardInfoLayer::create(data);
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
