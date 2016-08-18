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
#include "DeckData.h"
#include "CardSimpleData.h"
#include "DeckManager.h"
#include "TabButton.h"
#include "UniversalButton.h"
#include <unordered_set>

using namespace std;

static const int zorder_top(1);
static const float moveDuration(0.2f);

static const Vec2 primaryEdge(15, 15);
static const Vec2 secondaryEdge(5, 5);

static const float leftWidth(366);
static const float rightWidth(612);
static const float nodeHeight(496);

static const Size leftSubNodeSize(leftWidth - secondaryEdge.x * 2, 60);
#if DECKLAYER_ENABLE_TYPE_FILTER
static const Size rightSubNodeSize(rightWidth - secondaryEdge.x * 2, 92);
#else
static const Size rightSubNodeSize(rightWidth - secondaryEdge.x * 2, leftSubNodeSize.height);
#endif
static const Size scrollViewSize(rightSubNodeSize.width, nodeHeight- (rightSubNodeSize.height + secondaryEdge.y * 2 + 5));

static const int candidateColumn(6);
static const float cardSpaceX((scrollViewSize.width - DeckCard::Width * candidateColumn) / (candidateColumn + 1));
static const float cardSpaceY(15);
static const float foundSpaceTop(10);
static const float foundSpaceBottom(30);
static const float unfoundSpaceTop(20);
static const float unfoundSpaceBottom(115);

static const vector<DeckManager::SortType> sortTypes = {
    DeckManager::SortType::Default,
    DeckManager::SortType::Rarity,
    DeckManager::SortType::Elixir,
    DeckManager::SortType::Dungeon
};

static int getSortTypeIdx(DeckManager::SortType type)
{
    for (int i = 0; i < sortTypes.size(); ++i) {
        if (sortTypes.at(i) == type) {
            return i;
        }
    }
    
    return -1;
}

static string getSortTypeName(int idx)
{
    idx = MAX(MIN((int)sortTypes.size() - 1, idx), 0);
    
    auto type(sortTypes.at(idx));
    switch (type) {
        case DeckManager::SortType::Default:
            return LocalHelper::getString("ui_deck_sort_default");
        case DeckManager::SortType::Rarity:
            return LocalHelper::getString("ui_deck_sort_rarity");
        case DeckManager::SortType::Elixir:
            return LocalHelper::getString("ui_deck_sort_elixir");
        case DeckManager::SortType::Dungeon:
            return LocalHelper::getString("ui_deck_sort_dungeon");
        default:
            return "";
    }
}

#if DECKLAYER_ENABLE_TYPE_FILTER
static const vector<DeckTabType> cardTabs = {
    DeckTabType::All,
    DeckTabType::Heroes,
    DeckTabType::Soldiers,
    DeckTabType::Spells
};
#endif

template<typename _Key, typename _Value>
static void clearMap(unordered_map<_Key, _Value> &m)
{
    for(auto iter = begin(m); iter != end(m); ++iter) {
        _Value p = iter->second;
        Node* node = dynamic_cast<Node*>(p);
        if (node) {
            node->removeFromParent();
        } else {
            CC_SAFE_DELETE(p);
        }
    }
    m.clear();
}

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
,_cardsCountLabel(nullptr)
,_averageElixirLabel(nullptr)
,_sortTypeButton(nullptr)
,_scrollView(nullptr)
,_unfoundLine(nullptr)
,_opNode(nullptr)
,_deckEditMask(nullptr)
,_usedCard(nullptr)
,_usedCardPoint(Point::ZERO)
,_cardOriginalPoint(Point::ZERO)
,_isEditing(false)
,_thisSortIdx(getSortTypeIdx(DeckManager::getInstance()->getSortType())) {}

BattleDeckLayer::~BattleDeckLayer()
{
    _eventDispatcher->removeCustomEventListeners(DeckManager::SortNotification);
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
        auto bg = Sprite::create(BattleDeckUI::getResourcePath("ui_background_8.png"));
        bg->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(bg);
        _background = bg;
        
        CocosUtils::createRedExitButton(bg, [this]() {
            DeckManager::getInstance()->saveThisDeckData();
            removeFromParent();
        });
        
        const auto& size(bg->getContentSize());
        static const float titleEdgeY(32);
        auto title = CocosUtils::createLabel(LocalHelper::getString("ui_deck_battleDeck"), TITLE_FONT_SIZE);
        title->setAnchorPoint(Point::ANCHOR_MIDDLE);
        title->setPosition(Point(size.width / 2, size.height - titleEdgeY));
        bg->addChild(title);
        
        createLeftNode();
        createRightNode();
        loadCandidates();
        updateCardsCount();
        
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
        
        _eventDispatcher->addCustomEventListener(DeckManager::SortNotification, [this](EventCustom *event) {
            onReceivedManagerNotifications(event->getEventName());
        });
        
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
        auto parent(_deckCards.front()->getParent());
        const auto& point(parent->convertToNodeSpace(touch->getLocation()));
        bool touched(false);
        for (auto card : _deckCards) {
            if (card->getBoundingBox().containsPoint(point)) {
                touched = true;
                break;
            }
        }
        
        if (!touched) {
            useCardCancelled();
        }
    }
}

#pragma mark - DeckCardObserver
void BattleDeckLayer::onDeckCardTouched(DeckCard* touchedCard, ui::Widget::TouchEventType type)
{
    if (touchedCard && _isEditing) {
        if (ui::Widget::TouchEventType::BEGAN == type) {
            _cardOriginalPoint = touchedCard->getPosition();
            touchedCard->setLocalZOrder(zorder_top);
        } else if (ui::Widget::TouchEventType::MOVED == type) {
            const auto& point(touchedCard->getParent()->convertToNodeSpace(touchedCard->getTouchMovePosition()));
            touchedCard->setPosition(point);
        } else {
            auto replacedCard(getIntersectedCard(touchedCard));
            if (touchedCard == _usedCard) {
                if (replacedCard) {
                    useCard(replacedCard, false);
                } else {
                    useCardCancelled();
                }
            } else {
                if (replacedCard) {
                    if (replacedCard == _usedCard) {
                        useCard(touchedCard, true);
                    } else {
                        exchangeCard(touchedCard, replacedCard);
                    }
                } else {
                    exchangeCardCancelled(touchedCard);
                }
            }
        }
    }
}

void BattleDeckLayer::onDeckCardClicked(DeckCard* pSender)
{
    if (_isEditing) {
        if (_usedCard && pSender != _usedCard) {
            useCard(pSender, false);
        }
    } else {
        CC_ASSERT(pSender);
        showOpNode(pSender);
    }
}

#pragma mark - DeckCardOpNodeObserver
void BattleDeckLayer::onDeckCardOpNodeClicked()
{
    hideOpNode();
}

void BattleDeckLayer::onDeckCardOpNodeClickedButton(DeckCardOpType type, int cardId)
{
    if (DeckCardOpType::Upgrade == type || DeckCardOpType::Info == type) {
        hideOpNode();
    } else if (DeckCardOpType::Use == type) {
        beginEdit(cardId);
    }
}

#pragma mark - DeckEditMaskObserver
void BattleDeckLayer::onDeckEditMaskTouched(const Point& point)
{
    if (!_usedCard || !getWorldBoundingBox(_usedCard).containsPoint(point)) {
        useCardCancelled();
    }
}

#pragma mark - UI
void BattleDeckLayer::createLeftNode()
{
    if (_background) {
        static const Size subSize(leftWidth, nodeHeight);
        auto node = CocosUtils::createSubBackground(subSize);
        node->setPosition(primaryEdge.x + subSize.width / 2, primaryEdge.y + subSize.height / 2);
        _background->addChild(node, zorder_top);
        
        static const Size& topBarSize(leftSubNodeSize);
        
        // top
        {
            auto bar = CocosUtils::createPureBar(topBarSize);
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
            static const int column(2);
            for (int i = 0; i < column; ++i) {
                static const float cardSpaceX(20);
                static const float basePosX((subSize.width - (column * DeckCard::Width + (column - 1) * cardSpaceX)) / 2);
                auto card = createCard(0);
                const float x = basePosX + (i + 0.5) * DeckCard::Width + i * cardSpaceX;
                const float y = line->getPositionY() - (line->getContentSize().height / 2 + spaceLineCard + DeckCard::Height / 2);
                card->setPosition(Point(x, y));
                node->addChild(card);
                _deckCards.push_back(card);
            }
        }
        
        static const Size bottomBarSize(topBarSize.width, 36);
        
        //
        {
            auto bar = CocosUtils::createPureBar(bottomBarSize);
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
                    beginEdit(0);
                }
            });
            button->setPosition(bottomBarSize.width - button->getContentSize().width / 2, bottomBarSize.height / 2);
            bar->addChild(button);
        }
        
        // soldiers
        {
            static const float spaceLineCard(13);
            static const float spaceBarCard(5);
            static const int column(4);
            static const int row(2);
            static const float cardSpaceY(13);
            
            static float posY(secondaryEdge.y + bottomBarSize.height + row * DeckCard::Height + (row - 1) * cardSpaceY + spaceLineCard + spaceBarCard);
            auto line = createLine(false);
            line->setPosition(subSize.width / 2, posY);
            node->addChild(line);
            
            for (int i = 0; i < row; ++i) {
                static const float basePosY(posY - spaceLineCard);
                for (int j = 0; j < column; ++j) {
                    static const float cardSpaceX(5);
                    static const float basePosX((subSize.width - (column * DeckCard::Width + (column - 1) * cardSpaceX)) / 2);
                    auto card = createCard(0);
                    const float x = basePosX + (j + 0.5) * DeckCard::Width + j * cardSpaceX;
                    const float y = basePosY - (i + 0.5) * DeckCard::Height - i * cardSpaceY;
                    card->setPosition(Point(x, y));
                    node->addChild(card);
                    _deckCards.push_back(card);
                }
            }
        }
    }
}

void BattleDeckLayer::createRightNode()
{
    if (_background) {
        const auto& size(_background->getContentSize());
        static const Size subSize(rightWidth, nodeHeight);
        auto node = CocosUtils::createSubBackground(subSize);
        node->setPosition(size.width - (primaryEdge.x + subSize.width / 2), primaryEdge.y + subSize.height / 2);
        _background->addChild(node);
        
        static const Size& topBarSize(rightSubNodeSize);
        
        // top
        {
            auto bar = CocosUtils::createPureBar(topBarSize);
            bar->setPosition(subSize.width / 2, subSize.height - (secondaryEdge.y + topBarSize.height / 2));
            node->addChild(bar);
            
            static const Vec2 thirdEdge(5, 5);
            {
                auto label = CocosUtils::createLabel("1", DEFAULT_FONT_SIZE);
                label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
                label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
                bar->addChild(label);
                _cardsCountLabel = label;
                
#if DECKLAYER_ENABLE_TYPE_FILTER
                const float y(topBarSize.height - (thirdEdge.y + label->getContentSize().height / 2));
#else
                const float y(topBarSize.height / 2);
#endif
                label->setPosition(thirdEdge.x, y);
            }
            
#if DECKLAYER_ENABLE_TYPE_FILTER
            float x(thirdEdge.x);
            for (int i = 0; i < cardTabs.size(); ++i) {
                static const auto normal(CocosUtils::getResourcePath("button_lanse_1.png"));
                static const auto selected(CocosUtils::getResourcePath("button_lvse_2.png"));
                const auto type(cardTabs.at(i));
                auto button = TabButton::create(getCardTabName(type), normal, selected, [this, type](Ref*) {
                    setCardType(type);
                });
                bar->addChild(button);
                _cardTabButtons.push_back(button);
                
                const auto& size = button->getContentSize();
                button->setPosition(x + size.width / 2, thirdEdge.y + size.height / 2);
                
                static const float spaceX(10);
                x += size.width + spaceX;
            }
#endif
            
            {
                _sortTypeButton = UniversalButton::create(UniversalButton::BSize::Big, UniversalButton::BType::Blue, getSortTypeName(_thisSortIdx));
                _sortTypeButton->setCallback([this](Ref*) {
                    setNextSortType();
                });
                bar->addChild(_sortTypeButton);
                
                auto label = CocosUtils::createLabel(LocalHelper::getString("ui_deck_sortType"), DEFAULT_FONT_SIZE);
                bar->addChild(label);
                
                const auto& ssize(_sortTypeButton->getContentSize());
#if DECKLAYER_ENABLE_TYPE_FILTER
                const float y(thirdEdge.y + ssize.height / 2);
                label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
                label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
                label->setPosition(topBarSize.width - (thirdEdge.x + ssize.width), topBarSize.height - (thirdEdge.y + label->getContentSize().height / 2));
#else
                const float y(topBarSize.height / 2);
                static const float space(5);
                label->setAlignment(TextHAlignment::RIGHT, TextVAlignment::CENTER);
                label->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
                label->setPosition(topBarSize.width - (thirdEdge.x + ssize.width + space), y);
#endif
                _sortTypeButton->setPosition(topBarSize.width - (thirdEdge.x + ssize.width / 2), y);
            }
        }
        
        // cards
        {
            _scrollView = ui::ScrollView::create();
            _scrollView->setDirection(ui::ScrollView::Direction::VERTICAL);
            _scrollView->setContentSize(scrollViewSize);
            _scrollView->setPosition(secondaryEdge);
            _scrollView->setInnerContainerSize(scrollViewSize);
            _scrollView->setScrollBarEnabled(false);
            _scrollView->addEventListener([this](Ref*, ui::ScrollView::EventType type) {
                if (ui::ScrollView::EventType::SCROLLING == type) {
                    if (_opNode && _opNode->isVisible() && _foundCards.find(_opNode->getCardId()) != end(_foundCards)) {
                        hideOpNode();
                    }
                }
            });
            node->addChild(_scrollView);
            
            _unfoundLine = createUnfoundLine();
            _scrollView->addChild(_unfoundLine);
        }
    }
}

DeckCard* BattleDeckLayer::createCard(int card)
{
    auto node = DeckCard::create(card);
    node->registerObserver(this);
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

Node* BattleDeckLayer::createUnfoundLine() const
{
    auto node = Node::create();
    
    auto line = Sprite::create(CocosUtils::getResourcePath("ui_line.png"));
    line->setScaleX(scrollViewSize.width / line->getContentSize().width);
    node->addChild(line);
    
    auto label = CocosUtils::createLabel(LocalHelper::getString("ui_deck_unfoundCards"), SMALL_FONT_SIZE);
    label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
    label->setAnchorPoint(Point::ANCHOR_MIDDLE);
    label->setTextColor(Color4B::BLACK);
    node->addChild(label);
    
    static const float spaceY(3);
    const float lineHeight(line->getContentSize().height);
    const float labelHeight(label->getContentSize().height);
    const Size size(scrollViewSize.width, lineHeight + labelHeight + spaceY);
    node->setAnchorPoint(Point::ANCHOR_MIDDLE);
    node->setContentSize(size);
    
    line->setPosition(Point(size.width / 2, size.height - (lineHeight / 2)));
    label->setPosition(Point(size.width / 2, labelHeight / 2));
    
    return node;
}

void BattleDeckLayer::updateCardsCount()
{
    if (_cardsCountLabel) {
        _cardsCountLabel->setString(LocalHelper::getString("ui_deck_progress") + StringUtils::format("%ld/%d", DeckManager::getInstance()->getAllFoundCardsCount(), 50));
    }
}

void BattleDeckLayer::updateAverageElixir()
{
    if (_averageElixirLabel) {
        float total(0);
        for (auto card : _deckCards) {
            auto data = DeckManager::getInstance()->getCardData(card->getCardId());
            if (data) {
                total += data->getCost();
            }
        }
        
        _averageElixirLabel->setString(LocalHelper::getString("ui_deck_elixirCost") + StringUtils::format("%.1f", total / 10.0f));
    }
}

#pragma mark - Oprations Node
void BattleDeckLayer::showOpNode(DeckCard* card)
{
    if (card) {
        if (_opNode && _opNode->isVisible() && card->getCardId() == _opNode->getCardId()) {
            return;
        }
        
        if (!_opNode) {
            _opNode = DeckCardOpNode::create();
            _opNode->registerObserver(this);
        } else {
            _opNode->setVisible(true);
        }
        
        _opNode->setCard(card->getCardId());
        
        auto parent(card->getParent());
        readdChild(parent, _opNode);
        _opNode->setLocalZOrder(zorder_top + 1);
        
        const bool inDeck(find(_deckCards, card));
        if (inDeck) {
            _opNode->setTypes({DeckCardOpType::Info});
        } else {
            _opNode->setTypes({DeckCardOpType::Info, DeckCardOpType::Use});
        }
        
        setOpNodePosition(card);
        
        if (!inDeck) {
            const auto& cpoint(card->getPosition());
            const auto& cheight(card->getContentSize().height);
            const float height(_scrollView->getInnerContainerSize().height);
            if (height > scrollViewSize.height) {
                const float offsetY(height - scrollViewSize.height);
                const float innerPositionY(_scrollView->getInnerContainerPosition().y);
                const float cwpY(parent->convertToWorldSpace(cpoint).y);
                
                static const float duration(0.5f);
                const float sb(_scrollView->getParent()->convertToWorldSpace(_scrollView->getPosition()).y);
                const float ob(unfoundSpaceBottom - (cwpY - cheight / 2 - sb));
                if (ob > 0) {
                    // innerPositionY is < 0
                    const float percentage(MAX(MIN(-1.0f * (innerPositionY + ob) / offsetY * 100.0f, 100), 0));
                    _scrollView->scrollToPercentVertical(100 - percentage, duration, true);
                } else {
                    const float st(sb + scrollViewSize.height);
                    const float ot(cwpY + cheight / 2 - st + foundSpaceTop);
                    if (ot > 0) {
                        // innerPositionY is < 0
                        const float percentage(MAX(MIN(-1.0f * (innerPositionY - ot) / offsetY * 100.0f, 100), 0));
                        _scrollView->scrollToPercentVertical(100 - percentage, duration, true);
                    }
                }
            }
        }
    }
}

void BattleDeckLayer::hideOpNode()
{
    if (_opNode) {
        _opNode->setVisible(false);
    }
}

bool BattleDeckLayer::setOpNodePosition(DeckCard* card)
{
    if (card && _opNode && _opNode->isVisible() && _opNode->getCardId() == card->getCardId()) {
        const auto& cpoint(card->getPosition());
        const auto& csize(card->getContentSize());
        const auto& nsize(_opNode->getContentSize());
        static const float offsetY(10);
        const auto npoint(cpoint + Point(0, (csize.height - nsize.height) / 2 + offsetY));
        _opNode->setPosition(npoint);
        
        return true;
    }
    
    return false;
}

void BattleDeckLayer::beginEdit(int cardId)
{
    CC_ASSERT(!_isEditing);
    _isEditing = true;
    hideOpNode();
    
    // create mask
    if (!_deckEditMask) {
        const auto& size(_background->getContentSize());
        const Size subSize(rightWidth, nodeHeight);
        const Point point(size.width - (primaryEdge.x + subSize.width), primaryEdge.y);
        
        _deckEditMask = DeckEditMask::create(subSize);
        _deckEditMask->registerObserver(this);
        _deckEditMask->setPosition(point);
        _background->addChild(_deckEditMask);
    }
    
    // create card
    if (_foundCards.find(cardId) != end(_foundCards)) {
        _usedCard = _foundCards.at(cardId);
        auto parent = (*_deckCards.begin())->getParent();
        readdChild(parent, _usedCard);
        _usedCardPoint = _usedCard->getPosition();
        
        auto dm(DeckManager::getInstance());
        auto ud(dm->getCardData(_usedCard->getCardId()));
        if (ud) {
            vector<DeckCard*> temp;
            for (auto card : _deckCards) {
                auto data(DeckManager::getInstance()->getCardData(card->getCardId()));
                if (data && data->isHero() == ud->isHero()) {
                    temp.push_back(card);
                }
            }
            shake({temp, {_usedCard}});
        } else { CC_ASSERT(false); }
    } else {
        _usedCard = nullptr;
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

void BattleDeckLayer::onReceivedManagerNotifications(const string& notification)
{
    if (DeckManager::SortNotification == notification) {
        sortCards(DeckManager::getInstance()->getFoundCards(), _foundCards, _foundPositions);
        sortCards(DeckManager::getInstance()->getUnfoundCards(), _unfoundCards, _unfoundPositions);
    }
}

void BattleDeckLayer::sortCards(const vector<int>& cards, unordered_map<int, DeckCard*>& nodes, const vector<Point>& positions)
{
    CC_ASSERT(cards.size() == nodes.size());
    for (int i = 0; i < cards.size(); ++i) {
        const auto cardId(cards.at(i));
        if (i < nodes.size()) {
            const auto& point(_foundPositions.at(i));
            if (nodes.find(cardId) != end(nodes)) {
                auto node(nodes.at(cardId));
                move(node, point, 0.3f, [=]() {
                    setOpNodePosition(node);
                });
            } else {
                auto node = createCard(cardId);
                node->setPosition(point);
                _scrollView->addChild(node);
                nodes.insert(make_pair(cardId, node));
            }
        } else { CC_ASSERT(false); }
    }
}

void BattleDeckLayer::exchangeCard(DeckCard* from, DeckCard* to)
{
    if (from && to) {
        auto dm(DeckManager::getInstance());
        auto fd(dm->getCardData(from->getCardId()));
        auto td(dm->getCardData(to->getCardId()));
        if (fd && td) {
            if (fd->isHero() == td->isHero()) {
                move(from, to->getPosition(), moveDuration, nullptr);
                move(to, _cardOriginalPoint, moveDuration, nullptr);
                
                exchange(_deckCards, from, to);
                
                DeckManager::getInstance()->exchangeCard(from->getCardId(), to->getCardId());
            } else {
                exchangeCardCancelled(from);
                MessageBox("英雄与士兵卡牌无法互换", nullptr);
            }
        } else { CC_ASSERT(false); }
    }
}

void BattleDeckLayer::exchangeCardCancelled(DeckCard* card)
{
    if (card) {
        move(card, _cardOriginalPoint, moveDuration, nullptr);
    }
}

void BattleDeckLayer::useCard(DeckCard* replaced, bool fromDeck)
{
    CC_ASSERT(_usedCard);
    if (_usedCard && replaced && replaced->getParent()) {
        const auto uid(_usedCard->getCardId());
        const auto rid(replaced->getCardId());
        auto dm(DeckManager::getInstance());
        auto ud(dm->getCardData(uid));
        auto rd(dm->getCardData(rid));
        if (ud && rd) {
            if (ud->isHero() == rd->isHero()) {
                readdChild(replaced->getParent(), _usedCard);
                
                Point point(Point::ZERO);
                if (fromDeck) {
                    point = _cardOriginalPoint;
                } else {
                    point = replaced->getPosition();
                }
                move(_usedCard, point, moveDuration, nullptr);
                
                readdChild(_scrollView, replaced);
                
                endEdit();
                
                replace(_deckCards, _usedCard, replaced);
                
                updateAverageElixir();
                
                if (_foundCards.find(uid) != end(_foundCards)) {
                    _foundCards.erase(uid);
                    _foundCards.insert(make_pair(rid, replaced));
                }
                
                DeckManager::getInstance()->useCard(uid, rid);
            } else {
                if (fromDeck) {
                    exchangeCardCancelled(replaced);
                } else {
                    useCardCancelled();
                }
                MessageBox("英雄与士兵卡牌无法互换", nullptr);
            }
        }
    }
}

void BattleDeckLayer::useCardCancelled()
{
    const bool isMoving(_usedCard && _usedCardPoint != _usedCard->getPosition());
    if (_usedCard) {
        move(_usedCard, _usedCardPoint, moveDuration, [=]() {
            readdChild(_scrollView, _usedCard);
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
void BattleDeckLayer::move(Node* node, const Point& point, float duration, const function<void()>& callback) const
{
    static const int actionTag(818);
    if (node) {
        if (node->getPosition() != point) {
            if (node->getActionByTag(actionTag)) {
                node->stopActionByTag(actionTag);
            }
            
            node->setLocalZOrder(zorder_top);
            auto action = Sequence::create(MoveTo::create(duration, point), CallFunc::create([=]() {
                node->setLocalZOrder(0);
                if (callback) {
                    callback();
                }
            }), nullptr);
            action->setTag(actionTag);
            node->runAction(action);
        } else if (callback) {
            callback();
        }
    }
}

void BattleDeckLayer::shake(const vector<vector<DeckCard*>>& nodes) const
{
    for (int i = 0; i < nodes.size(); ++i) {
        auto& set(nodes.at(i));
        for (auto iter = begin(set); iter != end(set); ++iter) {
            auto node(*iter);
            if (node) {
                node->shake();
            }
        }
    }
}

void BattleDeckLayer::stopShake()
{
    vector<vector<DeckCard*>> nodes = {
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

void BattleDeckLayer::readdChild(Node* parent, Node* child) const
{
    if (child && parent) {
        auto cp(child->getParent());
        if (cp != parent) {
            Point worldPoint(child->getPosition());
            if (cp) {
                child->retain();
                worldPoint = cp->convertToWorldSpace(worldPoint);
                child->removeFromParent();
            }
            parent->addChild(child);
            
            // reset position after "addChild",
            // because the parent of "child" may not be "parent"
            if (cp) {
                child->setPosition(child->getParent()->convertToNodeSpace(worldPoint));
                child->release();
            }
        }
    }
}

DeckCard* BattleDeckLayer::getIntersectedCard(const DeckCard* touchedCard) const
{
    if (touchedCard) {
        float unused(INT_MAX);
        auto card = getIntersectedCard(touchedCard, _deckCards, unused);
        
        if (card) {
            return card;
        } else if (_usedCard) {
            return getIntersectedCard(touchedCard, {_usedCard}, unused);
        }
    }
    
    return nullptr;
}

DeckCard* BattleDeckLayer::getIntersectedCard(const DeckCard* touchedCard, const vector<DeckCard*>& cards, float& intersectedArea) const
{
    if (nullptr == touchedCard || 0 == cards.size()) {
        return nullptr;
    }
    
    DeckCard* ret(nullptr);
    intersectedArea = INT_MAX;
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

float BattleDeckLayer::getHeight(size_t count, float spaceY) const
{
    if (count > 0) {
        const size_t row = (count - 1) / candidateColumn + 1;
        return row * (DeckCard::Height + spaceY) - spaceY;
    }
    
    return 0;
}

Point BattleDeckLayer::getPosition(int row, int column) const
{
    static const float cw(DeckCard::Width);
    static const float ch(DeckCard::Height);
    const float x = (column + 1) * (cardSpaceX + cw) - cw / 2;
    const float y = row * (ch + cardSpaceY) + ch / 2;
    return Point(x, y);
}

#pragma mark - Functions
bool BattleDeckLayer::find(const vector<DeckCard*>& cards, const DeckCard* data) const
{
    for (const DeckCard* type : cards) {
        if (type == data) {
            return true;
        }
    }
    
    return false;
}

bool BattleDeckLayer::replace(vector<DeckCard*>& cards, DeckCard* used, DeckCard* replaced) const
{
    if (used && replaced && used != replaced) {
        for (int i = 0; i < cards.size(); ++i) {
            if (replaced == cards.at(i)) {
                cards.at(i) = used;
                return true;
            }
        }
    }
    
    return false;
}

void BattleDeckLayer::exchange(vector<DeckCard*>& cards, DeckCard* from, DeckCard* to) const
{
    if (from && to && from != to) {
        int idxFrom(-1);
        int idxTo(-1);
        
        for (int i = 0; i < cards.size(); ++i) {
            if (from == cards.at(i)) {
                idxFrom = i;
            } else if (to == cards.at(i)) {
                idxTo = i;
            }
            
            if (idxFrom >= 0 && idxTo >= 0) {
                break;
            }
        }
        
        if (idxFrom >= 0 && idxTo >= 0) {
            cards.at(idxFrom) = to;
            cards.at(idxTo) = from;
        }
    }
}

void BattleDeckLayer::loadCandidates()
{
    clearMap(_foundCards);
    clearMap(_unfoundCards);
    _foundPositions.clear();
    
    const auto& found(DeckManager::getInstance()->getFoundCards());
    const auto& unfound(DeckManager::getInstance()->getUnfoundCards());
    
    const float width(_scrollView->getContentSize().width);
    const float h1 = getHeight(found.size(), cardSpaceY);
    const float h2 = getHeight(unfound.size(), cardSpaceY);
    const float fb(h2 > 0 ? foundSpaceBottom + _unfoundLine->getContentSize().height + unfoundSpaceTop : 0);
    const float height = MAX(scrollViewSize.height, (foundSpaceTop + h1 + h2) + fb + unfoundSpaceBottom);
    _scrollView->setInnerContainerSize(Size(width, height));
    
    // the found cards
    for (int i = 0; i < found.size(); ++i) {
        const auto cardId(found.at(i));
        auto card = createCard(cardId);
        _scrollView->addChild(card);
        _foundCards.insert(make_pair(cardId, card));
        
        auto point = getPosition(i / candidateColumn, i % candidateColumn) + Point(0, foundSpaceTop);
        card->setPosition(Point(point.x, height - point.y));
        _foundPositions.push_back(card->getPosition());
    }
    
    // hint
    _unfoundLine->setVisible(h2 > 0);
    
    if (h2 > 0) {
        _unfoundLine->setPosition(width / 2, h2 + unfoundSpaceTop + unfoundSpaceBottom + _unfoundLine->getContentSize().height / 2);
        
        // the unfound cards
        for (int i = 0; i < unfound.size(); ++i) {
            const auto cardId(unfound.at(i));
            auto card = createCard(cardId);
            _scrollView->addChild(card);
            _unfoundCards.insert(make_pair(cardId, card));
            
            const auto& point = getPosition(i / candidateColumn, i % candidateColumn);
            card->setPosition(Point(point.x, h2 + unfoundSpaceBottom - point.y));
            _unfoundPositions.push_back(card->getPosition());
        }
    }
}

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
        
        // update cards
        static const vector<DeckData::Type> types = {
            DeckData::Type::Hero, DeckData::Type::Soldier
        };
        size_t cnt(0);
        auto data(dm->getThisDeckData());
        bool find(false);
        for ( int i = 0; i < types.size(); ++i) {
            const auto& cards(data->getCards(types.at(i)));
            for (int j = 0; j < cards.size(); ++j) {
                const auto idx(j + cnt);
                if (idx < _deckCards.size()) {
                    const auto cardId(cards.at(j));
                    _deckCards.at(idx)->update(cardId);
                    if (_foundCards.find(cardId) != end(_foundCards)) {
                        _foundCards.at(cardId)->removeFromParent();
                        _foundCards.erase(cardId);
                    }
                    if (!find) {
                        find = setOpNodePosition(_deckCards.at(idx));
                    }
                } else { CC_ASSERT(false); }
            }
            cnt += cards.size();
        }
        
        if (!find) {
            hideOpNode();
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

void BattleDeckLayer::setNextSortType()
{
    int nextIdx = MAX(0, _thisSortIdx + 1);
    if (nextIdx >= sortTypes.size()) {
        nextIdx = 0;
    }
    
    _thisSortIdx = nextIdx;
    
    if (_sortTypeButton) {
        _sortTypeButton->setTitle(getSortTypeName(_thisSortIdx));
    }
    
    DeckManager::getInstance()->setSortType(sortTypes.at(_thisSortIdx));
}

void BattleDeckLayer::resetParams()
{
    _cardOriginalPoint = Point::ZERO;
    _usedCard = nullptr;
    _usedCardPoint = Point::ZERO;
}

#if DECKLAYER_ENABLE_TYPE_FILTER
void BattleDeckLayer::setCardType(DeckTabType type)
{
    if (_thisCardType != type) {
        _thisCardType = type;
        
        const auto info(_candidates.at(type));
        loadCandidates(info);
        
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
