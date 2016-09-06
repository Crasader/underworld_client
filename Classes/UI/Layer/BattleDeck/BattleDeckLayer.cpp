//
//  BattleDeckLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/10.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "BattleDeckLayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "BattleDeckUI.h"
#include "DataManager.h"
#include "DeckData.h"
#include "CardData.h"
#include "CardProperty.h"
#include "Board.h"
#include "PureScale9Sprite.h"
#include "TabButton.h"
#include "UniversalButton.h"

using namespace std;

static const int zorder_top(1);

#if DECKLAYER_ENABLE_TYPE_FILTER
static const vector<DeckTabType> cardTabs = {
    DeckTabType::All,
    DeckTabType::Heroes,
    DeckTabType::Soldiers,
    DeckTabType::Spells
};
#endif

#pragma mark - BattleDeckLayer
BattleDeckLayer* BattleDeckLayer::create()
{
    auto ret = new (nothrow) BattleDeckLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

BattleDeckLayer::BattleDeckLayer()
:_observer(nullptr)
,_background(nullptr)
#if DECKLAYER_ENABLE_TYPE_FILTER
,_thisCardType(DeckTabType::None)
#endif
,_averageElixirLabel(nullptr)
,_cardPreview(nullptr)
,_deckEditMask(nullptr)
,_usedCard(nullptr)
,_usedCardPoint(Point::ZERO)
,_featureType(DeckManager::FeatureType::Deck)
,_isEditing(false) {}

BattleDeckLayer::~BattleDeckLayer()
{
    CC_SAFE_DELETE(_cardPreview);
    removeAllChildren();
}

void BattleDeckLayer::registerObserver(BattleDeckLayerObserver *observer)
{
    _observer = observer;
}

#pragma mark - LayerColor
bool BattleDeckLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        auto board = Board::create(2);
        board->setTitle(LocalHelper::getString("ui_deck_battleDeck"));
        board->setExitCallback([this]() {
            DeckManager::getInstance()->saveThisDeckData();
            removeFromParent();
        });
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        _background = board;
        
        createLeftNode(board->getSubNode(0));
        createRightNode(board->getSubNode(1));
        
        // deck
        const int deckId(DeckManager::getInstance()->getThisDeckId());
        loadDeck(deckId);
#if DECKLAYER_ENABLE_TYPE_FILTER
        setCardType(DeckTabType::All);
#endif
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(BattleDeckLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(BattleDeckLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool BattleDeckLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void BattleDeckLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    if (_isEditing) {
        const auto& point(touch->getLocation());
        bool touched(false);
        for (auto card : _deckCards) {
            if (card && getWorldBoundingBox(card).containsPoint(point)) {
                touched = true;
                break;
            }
        }
        
        if (!touched) {
            useCardCancelled();
        }
    }
}

#pragma mark - BaseCardObserver
void BattleDeckLayer::onBaseCardTouched(BaseCard* touchedCard, ui::Widget::TouchEventType type)
{
    if (touchedCard && _isEditing) {
        if (ui::Widget::TouchEventType::BEGAN == type) {
            touchedCard->setLocalZOrder(zorder_top);
        } else if (ui::Widget::TouchEventType::MOVED == type) {
            const auto& point(touchedCard->getParent()->convertToNodeSpace(touchedCard->getTouchMovePosition()));
            touchedCard->setPosition(point);
        } else {
            auto replacedCard(getIntersectedCard(touchedCard));
            if (touchedCard == _usedCard) {
                if (replacedCard) {
                    useCard(replacedCard->getTag(), false);
                } else {
                    useCardCancelled();
                }
            } else {
                int idx(touchedCard->getTag());
                if (replacedCard) {
                    if (replacedCard == _usedCard) {
                        useCard(idx, true);
                    } else {
                        exchangeCard(idx, replacedCard->getTag());
                    }
                } else {
                    exchangeCardCancelled(idx);
                }
            }
        }
    }
}

void BattleDeckLayer::onBaseCardClicked(BaseCard* pSender)
{
    if (_isEditing) {
        if (_usedCard && pSender != _usedCard) {
            useCard(pSender->getTag(), false);
        }
    } else if (pSender) {
        auto data(pSender->getCardData());
        if (data && _cardPreview) {
            auto cardId(data->getId());
            const bool isCandidate(getFoundCard(cardId));
            vector<CardOpType> types = {CardOpType::Info};
            if (isCandidate) {
                types.push_back(CardOpType::Use);
            }
            _cardPreview->showOpNode(pSender, types);
        } else {
            showInfo(pSender->getCardId(), nullptr);
        }
    }
}

#pragma mark - DeckEditMaskObserver
void BattleDeckLayer::onDeckEditMaskTouched(const Point& point)
{
    if (!_usedCard || !getWorldBoundingBox(_usedCard).containsPoint(point)) {
        useCardCancelled();
    }
}

#pragma mark - CardPreviewObserver
BaseCard* BattleDeckLayer::onCardPreviewCreateCard(int cardId)
{
    return createCard(cardId);
}

void BattleDeckLayer::onCardPreviewClickedOpButton(CardOpType type, const AbstractData* data)
{
    CC_ASSERT(data);
    auto dm(DeckManager::getInstance());
    if (CardOpType::Use == type) {
        beginEdit(dm->getCardData(data->getId()));
    } else if (CardOpType::Info == type) {
        dm->getCardDetail(data->getId(), [this](const CardData* data) {
            showInfo(data->getId(), data);
        });
    }
}

#pragma mark - CardInfoLayerObserver
void BattleDeckLayer::onCardInfoLayerReturn(Node* pSender)
{
    if (pSender) {
        pSender->removeFromParent();
    }
}

void BattleDeckLayer::onCardInfoLayerExit(Node* pSender)
{
    if (pSender) {
        pSender->removeFromParent();
    }
    
    removeFromParent();
}

#pragma mark - SpellInfoLayerObserver
void BattleDeckLayer::onSpellInfoLayerExit(Node* pSender)
{
    if (pSender) {
        pSender->removeFromParent();
    }
}

void BattleDeckLayer::onSpellInfoLayerUpgrade(Node* pSender, const AbstractData* data)
{
    if (pSender) {
        pSender->removeFromParent();
    }
}

#pragma mark - UI
void BattleDeckLayer::createLeftNode(Node* node)
{
    if (_background && node) {
        node->setLocalZOrder(zorder_top);
        
        static const Vec2 secondaryEdge(5, 5);
        const auto& subSize(node->getContentSize());
        const Size topBarSize(subSize.width - secondaryEdge.x * 2, 60);
        
        // top
        {
            auto bar = PureScale9Sprite::create(PureScale9Sprite::Type::BlueDeep);
            bar->setContentSize(topBarSize);
            bar->setPosition(subSize.width / 2, subSize.height - (secondaryEdge.y + topBarSize.height / 2));
            node->addChild(bar);
            
            static const float edgeX(5);
            auto label = CocosUtils::createLabel(LocalHelper::getString("ui_deck_battleDeck"), DEFAULT_FONT_SIZE);
            label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            label->setPosition(edgeX, topBarSize.height / 2);
            bar->addChild(label);
            
            float x(edgeX);
            for (int i = 0; i < DeckManager::DecksMaxCount; ++i) {
                static const auto normal(CocosUtils::getResourcePath("button_lanse_2.png"));
                static const auto selected(CocosUtils::getResourcePath("button_lvse_3.png"));
                const auto idx(DeckManager::DecksMaxCount - i - 1);
                const string title = StringUtils::format("%d", idx + 1);
                auto button = TabButton::create(title, normal, selected, [this, idx](Ref*) {
                    if (!_isEditing) {
                        DeckManager::getInstance()->saveThisDeckData();
                        loadDeck(idx);
                    }
                });
                bar->addChild(button);
                _deckTabButtons.insert(make_pair(idx, button));
                
                const auto& size = button->getContentSize();
                button->setPosition(topBarSize.width - (x + size.width / 2), topBarSize.height / 2);
                
                static const float spaceX(10);
                x += size.width + spaceX;
            }
        }
        
        // heroes
        {
            static const float spaceBarLine(13);
            auto line = createLine(true);
            line->setPosition(subSize.width / 2, subSize.height - (secondaryEdge.y + topBarSize.height + spaceBarLine));
            node->addChild(line);
            
            static const float spaceLineCard(13);
            static const int column(DeckData::HeroCount);
            const float cardSpaceX((subSize.width - column * BaseCard::Width) / (column + 1));
            const float basePosX(cardSpaceX);
            for (int i = 0; i < column; ++i) {
                const float x = basePosX + (i + 0.5) * BaseCard::Width + i * cardSpaceX;
                const float y = line->getPositionY() - (line->getContentSize().height / 2 + spaceLineCard + BaseCard::Height / 2);
                node->addChild(initBaseCard(Point(x, y)));
            }
        }
        
        const Size bottomBarSize(topBarSize.width, 36);
        
        //
        {
            auto bar = PureScale9Sprite::create(PureScale9Sprite::Type::BlueDeep);
            bar->setContentSize(bottomBarSize);
            bar->setPosition(subSize.width / 2, bottomBarSize.height / 2 + secondaryEdge.y);
            node->addChild(bar, zorder_top);
            
            auto label = CocosUtils::createLabel("4.0", DEFAULT_FONT_SIZE);
            label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE);
            label->setPosition(bottomBarSize.width / 2, bottomBarSize.height / 2);
            bar->addChild(label);
            _averageElixirLabel = label;
            
            auto button = UniversalButton::create(UniversalButton::BSize::Small, UniversalButton::BType::Purple, LocalHelper::getString("ui_deck_move"));
            button->setCallback([this](Ref*) {
                if (!_isEditing) {
                    if (_cardPreview) {
                        _cardPreview->hideOpNode();
                    }
                    beginEdit(nullptr);
                }
            });
            button->setPosition(bottomBarSize.width - button->getContentSize().width / 2, bottomBarSize.height / 2);
            bar->addChild(button);
        }
        
        // soldiers
        {
            static const float spaceLineCard(13);
            static const float spaceBarCard(5);
            static const int row(2);
            static const int column((DeckData::SoldierCount - 1) / row + 1);
            static const float cardSpaceY(13);
            
            float posY(secondaryEdge.y + bottomBarSize.height + row * BaseCard::Height + (row - 1) * cardSpaceY + spaceLineCard + spaceBarCard);
            auto line = createLine(false);
            line->setPosition(subSize.width / 2, posY);
            node->addChild(line);
            
            const float basePosY(posY - spaceLineCard);
            const float cardSpaceX((subSize.width - column * BaseCard::Width) / (column + 1));
            const float basePosX(cardSpaceX);
            for (int i = 0; i < row; ++i) {
                for (int j = 0; j < column; ++j) {
                    const float x = basePosX + (j + 0.5) * BaseCard::Width + j * cardSpaceX;
                    const float y = basePosY - (i + 0.5) * BaseCard::Height - i * cardSpaceY;
                    node->addChild(initBaseCard(Point(x, y)));
                }
            }
        }
    }
}

void BattleDeckLayer::createRightNode(Node* node)
{
    if (_background && node && !_cardPreview) {
        _cardPreview = new (nothrow) CardPreview(_featureType, node, this);
    }
}

BaseCard* BattleDeckLayer::createCard(int card)
{
    auto node = BaseCard::create();
    node->registerObserver(this);
    node->update(card, DeckManager::getInstance()->getCardData(card));
    return node;
}

Node* BattleDeckLayer::createLine(bool isHero) const
{
    string file;
    string text;
    if (isHero) {
        file = "ui_line_1.png";
        text = LocalHelper::getString("ui_deck_tab_hero");
    } else {
        file = "ui_line_2.png";
        text = LocalHelper::getString("ui_deck_tab_soldier");
    }
    
    auto line = Sprite::create(CocosUtils::getResourcePath(file));
    auto label = CocosUtils::createLabel(text, DEFAULT_FONT_SIZE);
    label->setTextColor(Color4B::BLACK);
    label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
    label->setAnchorPoint(Point::ANCHOR_MIDDLE);
    label->setPosition(line->getContentSize().width / 2, line->getContentSize().height / 2);
    line->addChild(label);
    return line;
}

void BattleDeckLayer::updateAverageElixir()
{
    if (_averageElixirLabel) {
        float total(0);
        for (auto card : _deckCards) {
            if (card) {
                auto property(dynamic_cast<const CardProperty*>(card->getCardData()->getProperty()));
                if (property) {
                    total += property->getCost();
                }
            }
        }
        
        _averageElixirLabel->setString(LocalHelper::getString("ui_deck_elixirCost") + StringUtils::format("%.1f", total / DeckData::SoldierCount));
    }
}

#pragma mark - Info
void BattleDeckLayer::showInfo(int cardId, const CardData* data)
{
    auto property(dynamic_cast<const CardProperty*>(DataManager::getInstance()->getProperty(cardId)));
    if (property) {
        if (UnderWorld::Core::HMMCardClass::kHMMCardClass_Spell == property->getCardClass()) {
            auto layer = SpellInfoLayer::create(cardId, data);
            layer->registerObserver(this);
            addChild(layer);
        } else {
            auto layer = CardInfoLayer::create(cardId, data);
            layer->registerObserver(this);
            addChild(layer);
        }
    } else {
        CC_ASSERT(false);
    }
}

#pragma mark - Move cards
void BattleDeckLayer::beginEdit(const AbstractData* data)
{
    CC_ASSERT(!_isEditing);
    _isEditing = true;
    
    // create mask
    if (_background && !_deckEditMask) {
        auto node(_background->getSubNode(1));
        if (node) {
            const auto& size(node->getContentSize());
            _deckEditMask = DeckEditMask::create(size);
            _deckEditMask->registerObserver(this);
            _deckEditMask->setPosition(node->getPosition() - Point(size.width / 2, size.height / 2));
            _background->addChild(_deckEditMask);
        }
    }
    
    // create card
    const int cardId(data ? data->getId() : 0);
    _usedCard = getFoundCard(cardId);
    
    if (_usedCard) {
        BattleDeckUI::readdChild(_deckCards.front()->getParent(), _usedCard);
        _usedCardPoint = _usedCard->getPosition();
        
        vector<BaseCard*> temp;
        if (dynamic_cast<const CardProperty*>(data->getProperty())->isHero()) {
            for (int i = 0; i < DeckData::HeroCount; ++i) {
                temp.push_back(_deckCards.at(i));
            }
        } else {
            for (int i = DeckData::HeroCount; i < _deckCards.size(); ++i) {
                temp.push_back(_deckCards.at(i));
            }
        }
        temp.push_back(_usedCard);
        shake(temp);
    } else {
        shake({_deckCards});
    }
}

void BattleDeckLayer::endEdit()
{
    _isEditing = false;
    
    if (_deckEditMask) {
        _deckEditMask->removeFromParent();
        _deckEditMask = nullptr;
    }
    
    stopShake();
}

BaseCard* BattleDeckLayer::getFoundCard(int cardId) const
{
    if (_cardPreview) {
        return dynamic_cast<BaseCard*>(_cardPreview->getFoundCard(cardId));
    }
    
    return nullptr;
}

void BattleDeckLayer::exchangeCard(int idxFrom, int idxTo)
{
    if (isIdxValid(idxFrom) && isIdxValid(idxTo)) {
        auto dm(DataManager::getInstance());
        auto from(_deckCards.at(idxFrom));
        auto to(_deckCards.at(idxTo));
        auto fdata(dynamic_cast<const CardProperty*>(dm->getProperty(from->getCardId())));
        auto tdata(dynamic_cast<const CardProperty*>(dm->getProperty(to->getCardId())));
        CC_ASSERT(fdata && tdata);
        if (fdata->isHero() == tdata->isHero()) {
            DeckManager::getInstance()->exchangeCard(fdata->getId(), tdata->getId());
            moveToDeck(from, idxTo);
            moveToDeck(to, idxFrom);
        } else {
            exchangeCardCancelled(idxFrom);
            MessageBox("英雄与士兵卡牌无法互换", nullptr);
        }
    }
}

void BattleDeckLayer::exchangeCardCancelled(int idx)
{
    if (isIdxValid(idx)) {
        _deckCards.at(idx)->move(_deckPositions.at(idx), nullptr);
    }
}

void BattleDeckLayer::useCard(int idx, bool fromDeck)
{
    CC_ASSERT(_usedCard);
    if (_usedCard && isIdxValid(idx)) {
        auto dm(DataManager::getInstance());
        auto replaced(_deckCards.at(idx));
        auto udata(dynamic_cast<const CardProperty*>(dm->getProperty(_usedCard->getCardId())));
        auto rdata(dynamic_cast<const CardProperty*>(dm->getProperty(replaced->getCardId())));
        CC_ASSERT(udata && rdata);
        if (udata->isHero() == rdata->isHero()) {
            if (_cardPreview) {
                _cardPreview->readdToScrollView(replaced);
            }
            
            endEdit();
            
            const auto uid(udata->getId());
            const auto rid(rdata->getId());
            DeckManager::getInstance()->useCard(uid, rid);
            moveToDeck(_usedCard, idx);
            if (_cardPreview) {
                _cardPreview->removeFoundCard(uid, false);
                _cardPreview->insertFoundCard(rid, replaced);
                // this function will move the replaced card
                _cardPreview->sortAndRealign();
            }
            
            updateAverageElixir();
        } else {
            if (fromDeck) {
                exchangeCardCancelled(idx);
            } else {
                useCardCancelled();
            }
            MessageBox("英雄与士兵卡牌无法互换", nullptr);
        }
    }
}

void BattleDeckLayer::useCardCancelled()
{
    const bool isMoving(_usedCard && _usedCardPoint != _usedCard->getPosition());
    if (_usedCard) {
        _usedCard->move(_usedCardPoint, [=]() {
            if (_cardPreview) {
                _cardPreview->readdToScrollView(_usedCard);
            }
            
            if (isMoving) {
                resetParams();
            }
        });
    }
    
    endEdit();
    
    if (!isMoving) {
        resetParams();
    }
}

#pragma mark - Universal Methods
bool BattleDeckLayer::isIdxValid(int idx) const
{
    return idx >= 0 && idx < _deckPositions.size();
}

BaseCard* BattleDeckLayer::initBaseCard(const Point& point)
{
    auto card = createCard(0);
    card->setPosition(point);
    card->setTag(static_cast<int>(_deckCards.size()));
    _deckCards.push_back(card);
    _deckPositions.push_back(point);
    return card;
}

void BattleDeckLayer::moveToDeck(BaseCard* card, int idx)
{
    if (card && isIdxValid(idx)) {
        card->move(_deckPositions.at(idx), nullptr);
        card->setTag(idx);
        _deckCards.at(idx) = card;
    }
}

void BattleDeckLayer::shake(const vector<BaseCard*>& nodes) const
{
    for (auto node : nodes) {
        if (node) {
            node->shake();
        }
    }
}

void BattleDeckLayer::stopShake()
{
    vector<vector<BaseCard*>> nodes = {
        _deckCards, {_usedCard}
    };
    
    for (int i = 0; i < nodes.size(); ++i) {
        auto& set(nodes.at(i));
        for (auto iter = begin(set); iter != end(set); ++iter) {
            auto node(*iter);
            if (node) {
                node->stopShake();
            }
        }
    }
}

BaseCard* BattleDeckLayer::getIntersectedCard(const BaseCard* touchedCard) const
{
    if (touchedCard) {
        auto card = getIntersectedCard(touchedCard, _deckCards);
        if (card) {
            return card;
        } else if (_usedCard) {
            return getIntersectedCard(touchedCard, {_usedCard});
        }
    }
    
    return nullptr;
}

BaseCard* BattleDeckLayer::getIntersectedCard(const BaseCard* touchedCard, const vector<BaseCard*>& cards) const
{
    if (nullptr == touchedCard || 0 == cards.size()) {
        return nullptr;
    }
    
    BaseCard* ret(nullptr);
    float intersectedArea(INT_MAX);
    const auto& rect(getWorldBoundingBox(touchedCard));
    for (auto iter = begin(cards); iter != end(cards); ++iter) {
        const auto& bd(getWorldBoundingBox(*iter));
        if (touchedCard != (*iter) && bd.intersectsRect(rect)) {
            if (cards.size() == 1) {
                ret = *iter;
                break;
            } else {
                const auto& size = bd.unionWithRect(rect).size;
                float area = size.width * size.height;
                if (intersectedArea > area) {
                    intersectedArea = area;
                    ret = *iter;
                }
            }
        }
    }
    
    return ret;
}

Rect BattleDeckLayer::getWorldBoundingBox(const Node* node) const
{
    if (node && node->getParent()) {
        Rect rect(node->getBoundingBox());
        rect.origin = node->getParent()->convertToWorldSpace(rect.origin);
        return rect;
    }
    
    return Rect::ZERO;
}

#pragma mark - Functions
void BattleDeckLayer::loadDeck(int idx)
{
    if (true) {
        // tab buttons
        for (auto iter = begin(_deckTabButtons); iter != end(_deckTabButtons); ++iter) {
            iter->second->setEnabled(idx != iter->first);
        }
        
        // load data
        auto dm(DeckManager::getInstance());
        dm->loadDeck(idx);
        if (_cardPreview) {
            _cardPreview->sortAndRealign();
        }
        
        // update cards
        bool find(false);
        const auto& cards(dm->getThisDeckData()->getCards());
        for (int i = 0; i < cards.size(); ++i) {
            if (i < _deckCards.size()) {
                const auto cardId(cards.at(i));
                _deckCards.at(i)->update(cardId, dm->getCardData(cardId));
                if (_cardPreview) {
                    _cardPreview->removeFoundCard(cardId, true);
                }
                
                if (!find && _cardPreview) {
                    find = _cardPreview->setOpNodePosition(_deckCards.at(i));
                }
            } else { CC_ASSERT(false); }
        }
        
        if (!find && _cardPreview) {
            _cardPreview->hideOpNode();
        }
        
        for (int i = 0; i < _deckCards.size(); ++i) {
            auto card(_deckCards.at(i));
            static const int actionTag(1100);
            if (card->getActionByTag(actionTag)) {
                card->stopActionByTag(actionTag);
            }
            
            card->setVisible(false);
            auto action = Sequence::create(DelayTime::create(0.03 * (i + 1)), Show::create(), nullptr);
            action->setTag(actionTag);
            card->runAction(action);
        }
        
        updateAverageElixir();
    }
}

void BattleDeckLayer::resetParams()
{
    _usedCard = nullptr;
}

#if DECKLAYER_ENABLE_TYPE_FILTER
void BattleDeckLayer::setCardType(DeckTabType type)
{
    if (_thisCardType != type) {
        _thisCardType = type;
        
        const auto info(_candidates.at(type));
        loadCards(info);
        
        // tab buttons
        for (int i = 0; i < _cardTabButtons.size(); ++i) {
            _cardTabButtons.at(i)->setEnabled(type != cardTabs[i]);
        }
    }
}

string BattleDeckLayer::getCardTabName(DeckTabType type) const
{
    switch (type) {
        case DeckTabType::All:
            return LocalHelper::getString("ui_deck_tab_all");
        case DeckTabType::Heroes:
            return LocalHelper::getString("ui_deck_tab_hero");
        case DeckTabType::Soldiers:
            return LocalHelper::getString("ui_deck_tab_soldier");
        case DeckTabType::Spells:
            return LocalHelper::getString("ui_deck_tab_spell");
        default:
            return "";
    }
}
#endif
