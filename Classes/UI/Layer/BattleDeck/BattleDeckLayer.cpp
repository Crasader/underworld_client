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

#if DECKLAYER_ENABLE_TYPE_FILTER
static const vector<DeckTabType> cardTabs = {
    DeckTabType::All,
    DeckTabType::Heroes,
    DeckTabType::Soldiers,
    DeckTabType::Spells
};
#endif

enum class BattleDeckLayer::SortType {
    Default,
    Rarity,
    Elixir,
    Dungeon
};

template<typename _Type>
static void clearCardSet(unordered_set<_Type>& s)
{
    for (auto iter = begin(s); iter != end(s); ++iter) {
        Node* node = dynamic_cast<Node*>(*iter);
        if (node) {
            node->removeFromParent();
        } else {
            auto p = *iter;
            CC_SAFE_DELETE(p);
        }
    }
    
    s.clear();
}

#pragma mark - Candidate
class BattleDeckLayer::Candidate
{
public:
    Candidate() {}
    virtual ~Candidate() {
        for (int i = 0; i < foundCards.size(); ++i) {
            CC_SAFE_DELETE(foundCards.at(i));
        }
    }
    
public:
    vector<CardSimpleData*> foundCards;
    vector<int> unfoundCards;
};

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
#else
,_candidates(nullptr)
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
,_thisDeckData(nullptr)
,_isEditing(false)
,_thisSortIdx(0) {}

BattleDeckLayer::~BattleDeckLayer()
{
    removeAllChildren();
    
    clearCandidates();
    
    for (auto iter = begin(_editedDeckData); iter != end(_editedDeckData); ++iter) {
        CC_SAFE_DELETE(iter->second);
    }
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
        updateCardsCount(0);
        updateAverageElixir();
        
        // deck
        const auto idx(MAX(MIN(DeckManager::DecksMaxCount, DeckManager::getInstance()->getDefaultDeckId()), 1));
        loadDeck(idx);
        
        // scroll view
        initPositions();
        initCandidates();
#if DECKLAYER_ENABLE_TYPE_FILTER
        setCardType(DeckTabType::All);
#else
        loadCandidates(_candidates);
#endif
        
        if (true) {
            _thisSortIdx = -1;
            setNextSortType();
        }
        
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
                        move(touchedCard, replacedCard->getPosition(), nullptr);
                        move(replacedCard, _cardOriginalPoint, nullptr);
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
                const auto idx(DeckManager::DecksMaxCount - i);
                const string title = StringUtils::format("%d", idx);
                auto button = TabButton::create(title, normal, selected, [this, idx](Ref*) {
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
                auto card = createFoundCard(nullptr);
                card->setIsHero(true);
                card->setInDeck(true);
                const float x = basePosX + (i + 0.5) * DeckCard::Width + i * cardSpaceX;
                const float y = line->getPositionY() - (line->getContentSize().height / 2 + spaceLineCard + DeckCard::Height / 2);
                card->setPosition(Point(x, y));
                node->addChild(card);
                _heroCards.insert(card);
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
                    auto card = createFoundCard(nullptr);
                    card->setIsHero(false);
                    card->setInDeck(true);
                    const float x = basePosX + (j + 0.5) * DeckCard::Width + j * cardSpaceX;
                    const float y = basePosY - (i + 0.5) * DeckCard::Height - i * cardSpaceY;
                    card->setPosition(Point(x, y));
                    node->addChild(card);
                    _nonheroCards.insert(card);
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
                _sortTypeButton = UniversalButton::create(UniversalButton::BSize::Big, UniversalButton::BType::Blue, "By Rarity");
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

DeckCard* BattleDeckLayer::createFoundCard(const CardSimpleData* data)
{
    auto card = DeckCard::create(data);
    card->registerObserver(this);
    return card;
}

Node* BattleDeckLayer::createUnfoundCard(int cid) const
{
    auto card = DeckCard::create(nullptr);
    return card;
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
        if (_heroCards.find(card) != end(_heroCards) ||
            _nonheroCards.find(card) != end(_nonheroCards)) {
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
        auto parent = (*_heroCards.begin())->getParent();
        readdChild(parent, pickedCard);
        _usedCardPoint = pickedCard->getPosition();
        
        shake({_heroCards, _nonheroCards, {_usedCard}});
    } else {
        _usedCard = nullptr;
        shake({_heroCards, _nonheroCards});
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

void BattleDeckLayer::updateCardsCount(int count)
{
    if (_cardsCountLabel) {
        _cardsCountLabel->setString(LocalHelper::getString("ui_deck_progress") + StringUtils::format("%d/%d", count, 50));
    }
}

void BattleDeckLayer::updateAverageElixir()
{
    if (_averageElixirLabel) {
        _averageElixirLabel->setString(LocalHelper::getString("ui_deck_elixirCost") + StringUtils::format("%.1f", 4.0f));
    }
}

void BattleDeckLayer::exchangeCard(DeckCard* from, DeckCard* to)
{
    
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
        
        if (_heroCards.find(replaced) != end(_heroCards)) {
            _heroCards.erase(replaced);
            _heroCards.insert(touchedCard);
        } else if (_nonheroCards.find(replaced) != end(_nonheroCards)) {
            _nonheroCards.erase(replaced);
            _nonheroCards.insert(touchedCard);
        }
        
        _foundCards.insert(replaced);
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

void BattleDeckLayer::shake(const vector<unordered_set<DeckCard*>>& nodes)
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
    vector<unordered_set<DeckCard*>> nodes = {
        _heroCards, _nonheroCards, {_usedCard}
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
        float harea(INT_MAX);
        auto hero = getIntersectedCard(touchedCard, _heroCards, harea);
        
        float narea(INT_MAX);
        auto nonhero = getIntersectedCard(touchedCard, _nonheroCards, narea);
        
        if (hero || nonhero) {
            return (harea < narea) ? hero : nonhero;
        } else if (_usedCard) {
            return getIntersectedCard(touchedCard, {_usedCard}, narea);
        }
    }
    
    return nullptr;
}

DeckCard* BattleDeckLayer::getIntersectedCard(const DeckCard* touchedCard, const unordered_set<DeckCard*>& cards, float& intersectedArea) const
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
void BattleDeckLayer::initPositions()
{
    _foundPositions.clear();
    
    const auto cnt = DeckManager::getInstance()->getFoundCards().size();
    for (int i = 0; i < cnt; ++i) {
        auto pos = getPosition(i / candidateColumn, i % candidateColumn);
        _foundPositions.push_back(pos + Point(0, foundSpaceTop));
    }
}

void BattleDeckLayer::initCandidates()
{
    clearCandidates();
    
#if DECKLAYER_ENABLE_TYPE_FILTER
    for (int i = 0; i < cardTabs.size(); ++i) {
        _candidates.insert(make_pair(cardTabs.at(i), new (nothrow) Candidate()));
    }
#else
    _candidates = new (nothrow) Candidate();
#endif
    
    {
        const auto& found = DeckManager::getInstance()->getFoundCards();
        for (auto it = begin(found); it != end(found); ++it) {
#if DECKLAYER_ENABLE_TYPE_FILTER
            // TODO: remove test code
            for (auto iter = begin(_candidates); iter != end(_candidates); ++iter) {
                auto candidate(iter->second);
                auto data(*it);
                candidate->foundCards.push_back(new (nothrow) CardSimpleData(data));
                candidate->unfoundCards.push_back(data->getName());
            }
#else
            _candidates->foundCards.push_back(new (nothrow) CardSimpleData(it->second));
#endif
        }
    }
    
    {
        const auto& unfound = DeckManager::getInstance()->getUnfoundCards();
        for (auto it = begin(unfound); it != end(unfound); ++it) {
            _candidates->unfoundCards.push_back(*it);
        }
    }
}

void BattleDeckLayer::clearCandidates()
{
#if DECKLAYER_ENABLE_TYPE_FILTER
    for (auto iter = begin(_candidates); iter != end(_candidates); ++iter) {
        CC_SAFE_DELETE(iter->second);
    }
    
    _candidates.clear();
#else
    CC_SAFE_DELETE(_candidates);
#endif
}

void BattleDeckLayer::loadCandidates(Candidate* candidates)
{
    clearCardSet(_foundCards);
    clearCardSet(_unfoundCards);
    
    const auto& found(candidates->foundCards);
    const auto& unfound(candidates->unfoundCards);
    
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
        _foundCards.insert(card);
        
        const auto& point(_foundPositions.at(i));
        card->setPosition(Point(point.x, height - point.y));
    }
    
    // hint
    _unfoundLine->setVisible(h2 > 0);
    
    if (h2 > 0) {
        _unfoundLine->setPosition(width / 2, h2 + unfoundSpaceTop + unfoundSpaceBottom + _unfoundLine->getContentSize().height / 2);
        
        // the unfound cards
        for (int i = 0; i < unfound.size(); ++i) {
            auto node = createUnfoundCard(unfound.at(i));
            (dynamic_cast<DeckCard*>(node))->setIsHero(true);
            _scrollView->addChild(node);
            _unfoundCards.insert(node);
            
            const auto& point = getPosition(i / candidateColumn, i % candidateColumn);
            node->setPosition(Point(point.x, h2 + unfoundSpaceBottom - point.y));
        }
    }
}

void BattleDeckLayer::saveThisDeck()
{
    auto dm(DeckManager::getInstance());
    dm->setDeckData(_thisDeckIdx, _thisDeckData);
    dm->saveDeckData(_thisDeckIdx);
}

void BattleDeckLayer::loadDeck(int idx)
{
    if (_thisDeckIdx != idx) {
        _thisDeckIdx = idx;
        
        // tab buttons
        for (auto iter = begin(_deckTabButtons); iter != end(_deckTabButtons); ++iter) {
            iter->second->setEnabled(idx != iter->first);
        }
        
        if (_editedDeckData.find(idx) == end(_editedDeckData)) {
            auto dd = DeckManager::getInstance()->getDeckData(idx);
            _editedDeckData.insert(make_pair(idx, new (nothrow) DeckData(dd)));
        }
        
        _thisDeckData = _editedDeckData.at(idx);
        
        // heroes
        
        
        // soldiers
    }
}

void BattleDeckLayer::setDefaultDeck()
{
    DeckManager::getInstance()->setDefaultDeckId(_thisDeckIdx);
}

void BattleDeckLayer::setNextSortType()
{
    static const vector<BattleDeckLayer::SortType> types = {
        BattleDeckLayer::SortType::Default,
        BattleDeckLayer::SortType::Rarity,
        BattleDeckLayer::SortType::Elixir,
        BattleDeckLayer::SortType::Dungeon
    };
    
    int nextIdx = _thisSortIdx + 1;
    if (nextIdx >= types.size()) {
        nextIdx = 0;
    }
    
    _thisSortIdx = nextIdx;
    
    if (_sortTypeButton) {
        _sortTypeButton->setTitle(getSortTypeName(types.at(nextIdx)));
    }
    
    // TODO: sort
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

string BattleDeckLayer::getSortTypeName(SortType type) const
{
    switch (type) {
        case SortType::Default:
            return LocalHelper::getString("ui_deck_sort_default");
        case SortType::Rarity:
            return LocalHelper::getString("ui_deck_sort_rarity");
        case SortType::Elixir:
            return LocalHelper::getString("ui_deck_sort_elixir");
        case SortType::Dungeon:
            return LocalHelper::getString("ui_deck_sort_dungeon");
        default:
            return "";
    }
}
