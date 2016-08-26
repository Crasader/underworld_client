//
//  CardPreview.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardPreview.h"
#include "AbstractCard.h"
#include "UniversalButton.h"
#include "PureScale9Sprite.h"
#include "CardSet.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "CardSimpleData.h"

using namespace std;
using namespace ui;

static const Vec2 secondaryEdge(5, 5);
static const float cardSpaceY(15);
static const float foundSpaceTop(10);
static const float foundSpaceBottom(30);
static const float unfoundSpaceTop(20);

static int column(0);
static float unfoundSpaceBottom(0);

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

CardPreview::CardPreview(DeckManager::FeatureType type, Node* parent, CardPreviewObserver* observer)
:_observer(observer)
,_cardsCountLabel(nullptr)
,_sortTypeButton(nullptr)
,_scrollView(nullptr)
,_unfoundLine(nullptr)
,_foundCards(nullptr)
,_unfoundCards(nullptr)
,_opNode(nullptr)
,_featureType(type)
,_cardSize(Size::ZERO)
,_sortIdx(-1)
,_cardSpaceX(0)
,_scrollViewMinSize(Size::ZERO)
{
    if (!parent || !observer) {
        return;
    }
    
    // set some constants
    if (DeckManager::FeatureType::Deck == type) {
        column = 6;
        unfoundSpaceBottom = 115.0f;
    } else {
        column = 10;
        unfoundSpaceBottom = 50.0f;
    }
    
    const auto& size(parent->getContentSize());
    const Size barSize(size.width - secondaryEdge.x * 2, 60);
    auto dm(DeckManager::getInstance());
    _sortIdx = getSortTypeIdx(dm->getSortType(type));
    
    // top
    {
        auto bar = PureScale9Sprite::create(PureScale9Sprite::Type::BlueDeep);
        bar->setContentSize(barSize);
        bar->setPosition(size.width / 2, size.height - (secondaryEdge.y + barSize.height / 2));
        parent->addChild(bar);
        
        static const Vec2 thirdEdge(5, 5);
        {
            auto label = CocosUtils::createLabel("1", DEFAULT_FONT_SIZE);
            label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            bar->addChild(label);
            _cardsCountLabel = label;
            
            label->setPosition(thirdEdge.x, barSize.height / 2);
        }
        
        {
            _sortTypeButton = UniversalButton::create(UniversalButton::BSize::Big, UniversalButton::BType::Blue, getSortTypeName(_sortIdx));
            _sortTypeButton->setCallback([this](Ref*) {
                setNextSortType();
            });
            bar->addChild(_sortTypeButton);
            
            auto label = CocosUtils::createLabel(LocalHelper::getString("ui_deck_sortType"), DEFAULT_FONT_SIZE);
            bar->addChild(label);
            
            const auto& ssize(_sortTypeButton->getContentSize());
            const float y(barSize.height / 2);
            static const float space(5);
            label->setAlignment(TextHAlignment::RIGHT, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
            label->setPosition(barSize.width - (thirdEdge.x + ssize.width + space), y);
            _sortTypeButton->setPosition(barSize.width - (thirdEdge.x + ssize.width / 2), y);
        }
    }
    
    // cards
    {
        _scrollViewMinSize = Size(size.width, size.height - (barSize.height + secondaryEdge.y * 2 + 5));
        _scrollView = ScrollView::create();
        _scrollView->setDirection(ScrollView::Direction::VERTICAL);
        _scrollView->setContentSize(_scrollViewMinSize);
        _scrollView->setPosition(Point(0, secondaryEdge.y));
        _scrollView->setInnerContainerSize(_scrollViewMinSize);
        _scrollView->setScrollBarEnabled(false);
        _scrollView->addEventListener([this](Ref*, ui::ScrollView::EventType type) {
            if (ui::ScrollView::EventType::SCROLLING == type) {
                if (_opNode && _opNode->isVisible() && _foundCards->getCard(_opNode->getCardId())) {
                    hideOpNode();
                }
            }
        });
        parent->addChild(_scrollView);
    }
    
    if (observer) {
        _cardSize = observer->onCardPreviewCreateCard(0)->getContentSize();
        _cardSpaceX = (_scrollViewMinSize.width - _cardSize.width * column) / (column + 1);
    }
    
    dm->setSortType(_featureType, sortTypes.at(_sortIdx));
    initCards();
    updateCardsCount();
}

CardPreview::~CardPreview() {}

#pragma mark - public
void CardPreview::sortAndRealign()
{
    auto dm(DeckManager::getInstance());
    dm->setSortType(_featureType, sortTypes.at(_sortIdx));
    realignCards(dm->getFoundCards(_featureType), _foundCards);
    realignCards(dm->getUnfoundCards(), _unfoundCards);
}

void CardPreview::readdToScrollView(AbstractCard* card)
{
    BattleDeckUI::readdChild(_scrollView, card);
}

AbstractCard* CardPreview::getFoundCard(int cardId) const
{
    if (_foundCards) {
        return _foundCards->getCard(cardId);
    }
    
    return nullptr;
}

void CardPreview::removeFoundCard(int cardId, bool cleanup)
{
    if (_foundCards) {
        _foundCards->removeCard(cardId, cleanup);
    }
}

void CardPreview::insertFoundCard(int cardId, AbstractCard* card)
{
    if (_foundCards) {
        _foundCards->insertCard(cardId, card);
    }
}

#pragma mark - Oprations Node
void CardPreview::showOpNode(AbstractCard* card, const vector<DeckCardOpType>& types)
{
    if (!isOpNodeOnCard(card)) {
        if (!_opNode) {
            _opNode = CardOpNode::create();
            _opNode->registerObserver(this);
        } else {
            _opNode->setVisible(true);
        }
        
        const int cardId(card->getCardId());
        _opNode->setCard(cardId);
        
        auto parent(card->getParent());
        BattleDeckUI::readdChild(parent, _opNode);
        _opNode->setLocalZOrder(2);
        _opNode->setTypes(types);
        setOpNodePosition(card);
        
        if (_foundCards->getCard(cardId)) {
            fullyDisplayCard(card);
        }
    }
}

void CardPreview::hideOpNode()
{
    if (_opNode) {
        _opNode->setVisible(false);
    }
}

bool CardPreview::setOpNodePosition(AbstractCard* card)
{
    if (isOpNodeOnCard(card)) {
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

#pragma mark - CardOpNodeObserver
void CardPreview::onCardOpNodeClicked()
{
    hideOpNode();
}

void CardPreview::onCardOpNodeClickedButton(DeckCardOpType type, int cardId)
{
    hideOpNode();
    
    if (_observer) {
        _observer->onCardPreviewClickedOpButton(type, cardId);
    }
}

#pragma mark - private
void CardPreview::initCards()
{
    if (!_unfoundLine) {
        auto line = createUnfoundLine();
        _scrollView->addChild(line);
        _unfoundLine = line;
    }
    
    auto dm(DeckManager::getInstance());
    auto& foundCards(dm->getFoundCards(_featureType));
    auto& unfoundCards(dm->getUnfoundCards());
    
    const float width(_scrollView->getContentSize().width);
    const float h1 = getHeight(foundCards.size(), cardSpaceY);
    const float h2 = getHeight(unfoundCards.size(), cardSpaceY);
    const float fb(h2 > 0 ? foundSpaceBottom + _unfoundLine->getContentSize().height + unfoundSpaceTop : 0);
    const float height = MAX(_scrollViewMinSize.height, (foundSpaceTop + h1 + h2) + fb + unfoundSpaceBottom);
    _scrollView->setInnerContainerSize(Size(width, height));
    
    // the found cards
    if (h1 > 0) {
        if (!_foundCards) {
            _foundCards = new (nothrow) CardSet();
        } else {
            _foundCards->clear();
        }
    }
    
    for (int i = 0; i < foundCards.size(); ++i) {
        const auto cardId(foundCards.at(i));
        auto card = createCardNode(cardId);
        _scrollView->addChild(card);
        _foundCards->insertCard(cardId, card);
        
        auto point = getPosition(i / column, i % column) + Point(0, foundSpaceTop);
        card->setPosition(Point(point.x, height - point.y));
        _foundCards->pushPosition(card->getPosition());
    }
    
    // hint
    _unfoundLine->setVisible(h2 > 0);
    
    if (h2 > 0) {
        const float halfHeight(_unfoundLine->getContentSize().height / 2);
        _unfoundLine->setPosition(width / 2, height - (foundSpaceTop + h1 + foundSpaceBottom + halfHeight));
        
        // the unfound cards
        if (!_unfoundCards) {
            _unfoundCards = new (nothrow) CardSet();
        } else {
            _unfoundCards->clear();
        }
        
        for (int i = 0; i < unfoundCards.size(); ++i) {
            const auto cardId(unfoundCards.at(i));
            auto card = createCardNode(cardId);
            _scrollView->addChild(card);
            _unfoundCards->insertCard(cardId, card);
            
            const auto& point = getPosition(i / column, i % column);
            card->setPosition(Point(point.x, _unfoundLine->getPositionY() - (halfHeight + unfoundSpaceTop + point.y)));
            _unfoundCards->pushPosition(card->getPosition());
        }
    }
}

AbstractCard* CardPreview::createCardNode(int cardId)
{
    if (_observer) {
        return _observer->onCardPreviewCreateCard(cardId);
    }
    
    return nullptr;
}

Node* CardPreview::createUnfoundLine() const
{
    auto node = Node::create();
    
    auto line = Sprite::create(CocosUtils::getResourcePath("ui_line.png"));
    line->setScaleX(_scrollViewMinSize.width / line->getContentSize().width);
    node->addChild(line);
    
    auto label = CocosUtils::createLabel(LocalHelper::getString("ui_deck_unfoundCards"), SMALL_FONT_SIZE);
    label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
    label->setAnchorPoint(Point::ANCHOR_MIDDLE);
    label->setTextColor(Color4B::BLACK);
    node->addChild(label);
    
    static const float spaceY(3);
    const float lineHeight(line->getContentSize().height);
    const float labelHeight(label->getContentSize().height);
    const Size size(_scrollViewMinSize.width, lineHeight + labelHeight + spaceY);
    node->setAnchorPoint(Point::ANCHOR_MIDDLE);
    node->setContentSize(size);
    
    line->setPosition(Point(size.width / 2, size.height - (lineHeight / 2)));
    label->setPosition(Point(size.width / 2, labelHeight / 2));
    
    return node;
}

string CardPreview::getSortTypeName(int idx) const
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

void CardPreview::setNextSortType()
{
    int nextIdx = MAX(0, _sortIdx + 1);
    if (nextIdx >= sortTypes.size()) {
        nextIdx = 0;
    }
    
    _sortIdx = nextIdx;
    
    if (_sortTypeButton) {
        _sortTypeButton->setTitle(getSortTypeName(_sortIdx));
    }
    
    sortAndRealign();
}

void CardPreview::realignCards(const vector<int>& cards, CardSet* cardSet)
{
    if (cards.empty() || !cardSet) {
        return;
    }
    
    CC_ASSERT(cardSet && cards.size() == cardSet->getCardsCount());
    for (int i = 0; i < cards.size(); ++i) {
        const auto cardId(cards.at(i));
        if (i < cardSet->getCardsCount()) {
            const auto& point(cardSet->getPosition(i));
            auto card(cardSet->getCard(cardId));
            if (card) {
                card->move(point, [this, card]() {
                    setOpNodePosition(card);
                });
            } else {
                card = createCardNode(cardId);
                card->setPosition(point);
                _scrollView->addChild(card);
                cardSet->insertCard(cardId, card);
            }
        } else { CC_ASSERT(false); }
    }
}

float CardPreview::getHeight(size_t count, float spaceY) const
{
    if (count > 0) {
        const size_t row = (count - 1) / column + 1;
        return row * (_cardSize.height + spaceY) - spaceY;
    }
    
    return 0;
}

Point CardPreview::getPosition(int row, int column) const
{
    const float cw(_cardSize.width);
    const float ch(_cardSize.height);
    const float x = (column + 1) * (_cardSpaceX + cw) - cw / 2;
    const float y = row * (ch + cardSpaceY) + ch / 2;
    return Point(x, y);
}

void CardPreview::updateCardsCount()
{
    if (_cardsCountLabel) {
        auto dm(DeckManager::getInstance());
        _cardsCountLabel->setString(LocalHelper::getString("ui_deck_progress") + StringUtils::format("%ld/%ld", dm->getAllFoundCardsCount(), dm->getAllCardsCount()));
    }
}

void CardPreview::fullyDisplayCard(AbstractCard* card)
{
    if (card && card->getParent()) {
        const auto& cpoint(card->getPosition());
        const auto& cheight(card->getContentSize().height);
        const float height(_scrollView->getInnerContainerSize().height);
        if (height > _scrollViewMinSize.height) {
            const float offsetY(height - _scrollViewMinSize.height);
            const float innerPositionY(_scrollView->getInnerContainerPosition().y);
            const float cwpY(card->getParent()->convertToWorldSpace(cpoint).y);
            
            static const float duration(0.5f);
            const float sb(_scrollView->getParent()->convertToWorldSpace(_scrollView->getPosition()).y);
            const float ob(unfoundSpaceBottom - (cwpY - cheight / 2 - sb));
            if (ob > 0) {
                // innerPositionY is < 0
                const float percentage(MAX(MIN(-1.0f * (innerPositionY + ob) / offsetY * 100.0f, 100), 0));
                _scrollView->scrollToPercentVertical(100 - percentage, duration, true);
            } else {
                const float st(sb + _scrollViewMinSize.height);
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

bool CardPreview::isOpNodeOnCard(AbstractCard* card) const
{
    return card && _opNode && _opNode->isVisible() && _opNode->getCardId() == card->getCardId();
}
