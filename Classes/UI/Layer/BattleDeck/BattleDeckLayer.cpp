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
static const float unfoundSpaceBottom(40);

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

template<typename _Type>
static void clearVector(vector<_Type>& v)
{
    for (auto iter = begin(v); iter != end(v); ++iter) {
        Node* node = dynamic_cast<Node*>(*iter);
        if (node) {
            node->removeFromParent();
        } else {
            auto p = *iter;
            CC_SAFE_DELETE(p);
        }
    }
    
    v.clear();
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
,_editDeckMask(nullptr)
,_cardOriginalPoint(Point::ZERO)
,_usedCard(nullptr)
,_usedCardPoint(Point::ZERO)
,_thisDeckIdx(-1)
,_isEditing(false)
,_thisSortIdx(getSortTypeIdx(DeckManager::getInstance()->getSortType())) {}

BattleDeckLayer::~BattleDeckLayer()
{
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
        updateCardsCount();
        updateAverageElixir();
        
        // deck
        const int deckId(DeckManager::getInstance()->getThisDeckId());
        loadDeck(deckId);
        
        // scroll view
        loadCandidates();
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
                    useCard(touchedCard, replacedCard, replacedCard->getPosition());
                } else {
                    useCardCancelled();
                }
            } else {
                if (replacedCard) {
                    if (replacedCard == _usedCard) {
                        useCard(replacedCard, touchedCard, _cardOriginalPoint);
                    } else {
                        exchangeCard(touchedCard, replacedCard);
                    }
                } else {
                    move(touchedCard, _cardOriginalPoint, nullptr);
                }
            }
        }
    }
}

void BattleDeckLayer::onDeckCardClicked(DeckCard* pSender)
{
    if (!_isEditing) {
        CC_ASSERT(pSender);
        showOpNode(pSender);
    }
}

#pragma mark - DeckCardOpNodeObserver
void BattleDeckLayer::onDeckCardOpNodeClickedButton(DeckCardOpType type, DeckCard* card)
{
    hideOpNode();
    
    if (DeckCardOpType::Upgrade == type || DeckCardOpType::Info == type) {
        
    } else if (DeckCardOpType::Use == type) {
        beginEdit(card);
    }
}

#pragma mark - EditDeckMaskObserver
void BattleDeckLayer::onEditDeckMaskTouched(const Point& point)
{
    if (!_usedCard || !getWorldBoundingBox(_usedCard).containsPoint(point)) {
        useCardCancelled();
    }
}

#pragma mark - public
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
                    DeckManager::getInstance()->saveThisDeckData();
                    loadDeck(idx);
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
                auto card = createFoundCard(0);
                card->setInDeck(true);
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
            node->addChild(bar);
            
            auto label = CocosUtils::createLabel("4.0", DEFAULT_FONT_SIZE);
            label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE);
            label->setPosition(bottomBarSize.width / 2, bottomBarSize.height / 2);
            bar->addChild(label);
            _averageElixirLabel = label;
            
            auto button = UniversalButton::create(UniversalButton::BSize::Small, UniversalButton::BType::Purple, LocalHelper::getString("ui_deck_move"));
            button->setCallback([this](Ref*) {
                beginEdit(nullptr);
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
                    auto card = createFoundCard(0);
                    card->setInDeck(true);
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
            node->addChild(_scrollView);
            
            _unfoundLine = createUnfoundLine();
            _scrollView->addChild(_unfoundLine);
        }
    }
}

DeckCard* BattleDeckLayer::createFoundCard(int card)
{
    auto node = DeckCard::create(card);
    node->registerObserver(this);
    return node;
}

Node* BattleDeckLayer::createUnfoundCard(int card) const
{
    auto node = DeckCard::create(0);
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

void BattleDeckLayer::showOpNode(DeckCard* card)
{
    if (card) {
        if (!_opNode) {
            _opNode = DeckCardOpNode::create();
            _opNode->registerObserver(this);
            addChild(_opNode);
        } else {
            _opNode->setVisible(true);
        }
        
        _opNode->setCard(card);
        if (find(_deckCards, card)) {
            _opNode->setTypes({DeckCardOpType::Info});
        } else {
            _opNode->setTypes({DeckCardOpType::Info, DeckCardOpType::Use});
        }
        
        const auto& point(convertToNodeSpace(card->getParent()->convertToWorldSpace(card->getPosition())));
        const auto& psize(card->getContentSize());
        const auto& nsize(_opNode->getContentSize());
        static const float offsetY(10);
        _opNode->setPosition(point + Point(0, (psize.height - nsize.height) / 2 + offsetY));
    }
}

void BattleDeckLayer::hideOpNode()
{
    if (_opNode) {
        _opNode->setVisible(false);
    }
}

void BattleDeckLayer::beginEdit(DeckCard* pickedCard)
{
    CC_ASSERT(!_isEditing);
    _isEditing = true;
    
    // create mask
    if (!_editDeckMask) {
        const auto& size(_background->getContentSize());
        const Size subSize(rightWidth, nodeHeight);
        const Point point(size.width - (primaryEdge.x + subSize.width), primaryEdge.y);
        
        _editDeckMask = EditDeckMask::create(subSize);
        _editDeckMask->registerObserver(this);
        _editDeckMask->setPosition(point);
        _background->addChild(_editDeckMask);
    }
    
    // create card
    if (pickedCard) {
        _usedCard = pickedCard;
        auto parent = (*_deckCards.begin())->getParent();
        readdChild(parent, pickedCard);
        _usedCardPoint = pickedCard->getPosition();
        
        shake({_deckCards, {_usedCard}});
    } else {
        _usedCard = nullptr;
        shake({_deckCards});
    }
}

void BattleDeckLayer::endEdit()
{
    _isEditing = false;
    
    if (_editDeckMask) {
        _editDeckMask->removeFromParent();
        _editDeckMask = nullptr;
    }
    
    stopShake();
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
        _averageElixirLabel->setString(LocalHelper::getString("ui_deck_elixirCost") + StringUtils::format("%.1f", 4.0f));
    }
}

void BattleDeckLayer::onReceivedManagerNotifications(const string& notification)
{
    if (DeckManager::SortNotification == notification) {
        const auto& found(DeckManager::getInstance()->getFoundCards());
        CC_ASSERT(found.size() == _foundCards.size());
        for (int i = 0; i < _foundCards.size(); ++i) {
            if (i < found.size()) {
                _foundCards.at(i)->update(found.at(i));
            }
        }
    }
}

void BattleDeckLayer::exchangeCard(DeckCard* from, DeckCard* to)
{
    if (from && to) {
        move(from, to->getPosition(), nullptr);
        move(to, _cardOriginalPoint, nullptr);
        
        exchange(_deckCards, from, to);
        
        DeckManager::getInstance()->exchangeCard(from->getCardId(), to->getCardId());
    }
}

void BattleDeckLayer::useCard(DeckCard* touchedCard, DeckCard* replaced, const Point& point)
{
    if (touchedCard && replaced && replaced->getParent()) {
        readdChild(replaced->getParent(), touchedCard);
        
        move(touchedCard, point, nullptr);
        
        // TODO: we need to sort the data to calculate the point
        move(replaced, _usedCardPoint, [=]() {
            readdChild(_scrollView, replaced);
        });
        
        endEdit();
        
        replace(_deckCards, touchedCard, replaced);
        // TODO:
        replace(_foundCards, replaced, touchedCard);
        
        DeckManager::getInstance()->useCard(touchedCard->getCardId(), replaced->getCardId());
    }
}

void BattleDeckLayer::useCardCancelled()
{
    if (_usedCard) {
        if (_usedCardPoint != _usedCard->getPosition()) {
            move(_usedCard, _usedCardPoint, [this]() {
                readdChild(_scrollView, _usedCard);
            });
        } else {
            readdChild(_scrollView, _usedCard);
        }
        
        _cardOriginalPoint = Point::ZERO;
        _usedCard = nullptr;
        _usedCardPoint = Point::ZERO;
    }
    
    endEdit();
}

void BattleDeckLayer::move(Node* node, const Point& point, const function<void()>& callback) const
{
    if (node) {
        static const float duration(0.2f);
        node->setLocalZOrder(zorder_top);
        node->runAction(Sequence::create(MoveTo::create(duration, point), CallFunc::create([=]() {
            node->setLocalZOrder(0);
            if (callback) {
                callback();
            }
        }), nullptr));
    }
}

void BattleDeckLayer::shake(const vector<vector<DeckCard*>>& nodes)
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
            child->retain();
            Point worldPoint(Point::ZERO);
            if (cp) {
                worldPoint = cp->convertToWorldSpace(child->getPosition());
            }
            child->removeFromParent();
            parent->addChild(child);
            child->setPosition(child->getParent()->convertToNodeSpace(worldPoint));
            child->release();
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

#pragma mark - functions
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
    clearVector(_foundCards);
    clearVector(_unfoundCards);
    _foundPositions.clear();
    
    const auto& found(DeckManager::getInstance()->getFoundCards());
    const auto& unfound(DeckManager::getInstance()->getUnfoundCards());
    
    const float width(_scrollView->getContentSize().width);
    const float h1 = getHeight(found.size(), cardSpaceY);
    const float h2 = getHeight(unfound.size(), cardSpaceY);
    const float fb(h1 > 0 ? foundSpaceBottom : 0);
    const float height = (foundSpaceTop + h1 + h2) + fb + (h2 > 0 ? (_unfoundLine->getContentSize().height + unfoundSpaceTop + unfoundSpaceBottom) : 0);
    _scrollView->setInnerContainerSize(Size(width, height));
    
    // the found cards
    for (int i = 0; i < found.size(); ++i) {
        auto card = createFoundCard(found.at(i));
        card->setInDeck(false);
        _scrollView->addChild(card);
        _foundCards.push_back(card);
        
        auto point = getPosition(i / candidateColumn, i % candidateColumn);
        card->setPosition(Point(point.x, height - point.y));
        _foundPositions.push_back(point + Point(0, foundSpaceTop));
    }
    
    // hint
    _unfoundLine->setVisible(h2 > 0);
    
    if (h2 > 0) {
        _unfoundLine->setPosition(width / 2, h2 + unfoundSpaceTop + unfoundSpaceBottom + _unfoundLine->getContentSize().height / 2);
        
        // the unfound cards
        for (int i = 0; i < unfound.size(); ++i) {
            auto node = createUnfoundCard(unfound.at(i));
            _scrollView->addChild(node);
            _unfoundCards.push_back(node);
            
            const auto& point = getPosition(i / candidateColumn, i % candidateColumn);
            node->setPosition(Point(point.x, h2 + unfoundSpaceBottom - point.y));
        }
    }
}

void BattleDeckLayer::loadDeck(int idx)
{
    if (_thisDeckIdx != idx) {
        _thisDeckIdx = idx;
        
        // tab buttons
        for (auto iter = begin(_deckTabButtons); iter != end(_deckTabButtons); ++iter) {
            iter->second->setEnabled(idx != iter->first);
        }
        
        DeckManager::getInstance()->loadDeck(idx);
        
        auto data(DeckManager::getInstance()->getThisDeckData());
        
        static const vector<DeckData::Type> types = {
            DeckData::Type::Hero, DeckData::Type::Soldier
        };
        size_t cnt(0);
        for ( int i = 0; i < types.size(); ++i) {
            const auto& cards(data->getCards(types.at(i)));
            for (int j = 0; j < cards.size(); ++j) {
                const auto idx(j + cnt);
                if (idx < _deckCards.size()) {
                    _deckCards.at(idx)->update(cards.at(j));
                }
            }
            cnt += cards.size();
        }
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
