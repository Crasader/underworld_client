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

static const Vec2 primaryEdge(15, 15);
static const Vec2 secondaryEdge(5, 5);

static const float leftWidth(366);
static const float rightWidth(612);
static const float nodeHeight(496);

static const Size leftSubNodeSize(leftWidth - secondaryEdge.x * 2, 60);
static const Size rightSubNodeSize(rightWidth - secondaryEdge.x * 2, 92);
static const Size scrollViewSize(rightSubNodeSize.width, nodeHeight- (rightSubNodeSize.height + secondaryEdge.y * 2 + 5));

static const int candidateColumn(6);
static const float cardSpaceX((scrollViewSize.width - BattleDeckCard::Width * candidateColumn) / (candidateColumn + 1));
static const float cardSpaceY(15);
static const float foundSpaceTop(10);
static const float foundSpaceBottom(30);
static const float unfoundSpaceTop(20);
static const float unfoundSpaceBottom(40);
static const vector<DeckTabType> cardTabs = {
    DeckTabType::All,
    DeckTabType::Heroes,
    DeckTabType::Soldiers,
    DeckTabType::Spells
};

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
    vector<string> unfoundCards;
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
,_cardsCountLabel(nullptr)
,_sortTypeButton(nullptr)
,_scrollView(nullptr)
,_unfoundLine(nullptr)
,_thisDeckIdx(-1)
,_thisDeckData(nullptr)
,_thisCardType(DeckTabType::None) {}

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
        auto title = CocosUtils::createLabel(LocalHelper::getString("ui_logShare_title"), BIG_FONT_SIZE);
        title->setAnchorPoint(Point::ANCHOR_MIDDLE);
        title->setPosition(Point(size.width / 2, size.height - titleEdgeY));
        bg->addChild(title);

        createLeftNode();
        createRightNode();
        updateCardsCount(0);
        
        // deck
        const auto idx(MAX(MIN(DeckManager::MaxCount, DeckManager::getInstance()->getDefaultDeckId()), 1));
        loadDeck(idx);
        
        // scroll view
        initPositions();
        initCandidates();
        setCardType(DeckTabType::All);
        
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

#pragma mark - BattleDeckCardObserver
void BattleDeckLayer::onBattleDeckCardTouched(BattleDeckCard* pSender)
{
    if (pSender) {
        const auto show(pSender->isShowHint());
        pSender->showHint(!show);
    }
}

void BattleDeckLayer::onBattleDeckCardUse(BattleDeckCard* pSender)
{
    
}

void BattleDeckLayer::onBattleDeckCardInfo(BattleDeckCard* pSender)
{
    
}

#pragma mark - public
void BattleDeckLayer::createLeftNode()
{
    if (_background) {
        static const Size subSize(leftWidth, nodeHeight);
        auto node = CocosUtils::createSubBackground(subSize);
        node->setPosition(primaryEdge.x + subSize.width / 2, primaryEdge.y + subSize.height / 2);
        _background->addChild(node);
        
        static const Size& subBgSize(leftSubNodeSize);
        auto subBg = CocosUtils::createPureBar(subBgSize);
        subBg->setPosition(subSize.width / 2, subSize.height - (secondaryEdge.y + subBgSize.height / 2));
        node->addChild(subBg);
        
        // top
        {
            static const float edgeX(5);
            auto label = CocosUtils::createLabel("Battle Deck", DEFAULT_FONT_SIZE);
            label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            label->setPosition(edgeX, subBgSize.height / 2);
            subBg->addChild(label);
            
            float x(edgeX);
            for (int i = 0; i < DeckManager::MaxCount; ++i) {
                static const auto normal(CocosUtils::getResourcePath("button_lanse_2.png"));
                static const auto selected(CocosUtils::getResourcePath("button_lvse_3.png"));
                const auto idx(DeckManager::MaxCount - i);
                const string title = StringUtils::format("%d", idx);
                auto button = TabButton::create(title, normal, selected, [this, idx](Ref*) {
                    loadDeck(idx);
                });
                subBg->addChild(button);
                _deckTabButtons.insert(make_pair(idx, button));
                
                const auto& size = button->getContentSize();
                button->setPosition(subBgSize.width - (x + size.width / 2), subBgSize.height / 2);
                
                static const float spaceX(10);
                x += size.width + spaceX;
            }
        }
        
        // heroes
        {
            static const float spaceY1(18);
            auto line = createLine(DeckTabType::Heroes);
            line->setPosition(subSize.width / 2, subBg->getPositionY() - (subBgSize.height / 2 + spaceY1));
            node->addChild(line);
            
            static const float spaceY2(18);
            static const int column(2);
            for (int i = 0; i < column; ++i) {
                static const float cardSpaceX(20);
                static const float basePosX((subSize.width - (column * BattleDeckCard::Width + (column - 1) * cardSpaceX)) / 2);
                auto card = createFoundCard(nullptr);
                card->setIsHero(true);
                card->setInDeck(true);
                const float x = basePosX + (i + 0.5) * BattleDeckCard::Width + i * cardSpaceX;
                const float y = line->getPositionY() - (line->getContentSize().height / 2 + spaceY2 + BattleDeckCard::Height / 2);
                card->setPosition(Point(x, y));
                node->addChild(card);
            }
        }
        
        // soldiers
        {
            static const float edgeBottom(8);
            static const float spaceY(33);
            static const int column(4);
            static const int row(2);
            static const float cardSpaceY(14);
            
            static float posY(edgeBottom + row * BattleDeckCard::Height + (row - 1) * cardSpaceY + spaceY);
            auto line = createLine(DeckTabType::Soldiers);
            line->setPosition(subSize.width / 2, posY);
            node->addChild(line);
            
            for (int i = 0; i < row; ++i) {
                static const float basePosY(posY - spaceY);
                for (int j = 0; j < column; ++j) {
                    static const float cardSpaceX(5);
                    static const float basePosX((subSize.width - (column * BattleDeckCard::Width + (column - 1) * cardSpaceX)) / 2);
                    auto card = createFoundCard(nullptr);
                    card->setIsHero(false);
                    card->setInDeck(true);
                    const float x = basePosX + (j + 0.5) * BattleDeckCard::Width + j * cardSpaceX;
                    const float y = basePosY - (i + 0.5) * BattleDeckCard::Height - i * cardSpaceY;
                    card->setPosition(Point(x, y));
                    node->addChild(card);
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
        
        static const Size& subBgSize(rightSubNodeSize);
        auto subBg = CocosUtils::createPureBar(subBgSize);
        subBg->setPosition(subSize.width / 2, subSize.height - (secondaryEdge.y + subBgSize.height / 2));
        node->addChild(subBg);
        
        // top
        {
            static const Vec2 thirdEdge(5, 5);
            
            {
                auto label = CocosUtils::createLabel("Card Type:", DEFAULT_FONT_SIZE);
                label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
                label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
                label->setPosition(thirdEdge.x, subBgSize.height - (thirdEdge.y + label->getContentSize().height / 2));
                subBg->addChild(label);
                _cardsCountLabel = label;
            }
            
            float x(thirdEdge.x);
            for (int i = 0; i < cardTabs.size(); ++i) {
                static const auto normal(CocosUtils::getResourcePath("button_lanse_1.png"));
                static const auto selected(CocosUtils::getResourcePath("button_lvse_2.png"));
                const auto type(cardTabs.at(i));
                auto button = TabButton::create(getCardTabName(type), normal, selected, [this, type](Ref*) {
                    setCardType(type);
                });
                subBg->addChild(button);
                _cardTabButtons.push_back(button);
                
                const auto& size = button->getContentSize();
                button->setPosition(x + size.width / 2, thirdEdge.y + size.height / 2);
                
                static const float spaceX(10);
                x += size.width + spaceX;
            }
            
            {
                _sortTypeButton = UniversalButton::create(UniversalButton::BSize::Big, UniversalButton::BType::Blue, "By Rarity");
                const auto& ssize(_sortTypeButton->getContentSize());
                _sortTypeButton->setPosition(subBgSize.width - (thirdEdge.x + ssize.width / 2), thirdEdge.y + ssize.height / 2);
                subBg->addChild(_sortTypeButton);
                
                auto label = CocosUtils::createLabel("Type :", DEFAULT_FONT_SIZE);
                label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
                label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
                label->setPosition(_sortTypeButton->getPositionX() - ssize.width / 2, subBgSize.height - (thirdEdge.y + label->getContentSize().height / 2));
                subBg->addChild(label);
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

BattleDeckCard* BattleDeckLayer::createFoundCard(const CardSimpleData* data)
{
    auto card = BattleDeckCard::create(data);
    card->registerObserver(this);
    return card;
}

Node* BattleDeckLayer::createUnfoundCard(const std::string& name) const
{
    auto card = BattleDeckCard::create(nullptr);
    return card;
}

Node* BattleDeckLayer::createLine(DeckTabType type) const
{
    string file;
    string text;
    if (DeckTabType::Heroes == type) {
        file = "ui_line_1.png";
        text = "Heroes";
    } else if (DeckTabType::Soldiers == type) {
        file = "ui_line_2.png";
        text = "Soldiers";
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
    
    auto label = CocosUtils::createLabel("Cards to be found", SMALL_FONT_SIZE);
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

void BattleDeckLayer::updateCardsCount(int count)
{
    if (_cardsCountLabel) {
        _cardsCountLabel->setString(LocalHelper::getString("ui_card_progress") + StringUtils::format("%d/%d", count, 50));
    }
}

float BattleDeckLayer::getHeight(size_t count, float spaceY) const
{
    if (count > 0) {
        const size_t row = (count - 1) / candidateColumn + 1;
        return row * (BattleDeckCard::Height + spaceY) - spaceY;
    }
    
    return 0;
}

Point BattleDeckLayer::getPosition(int row, int column) const
{
    static const float cw(BattleDeckCard::Width);
    static const float ch(BattleDeckCard::Height);
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
    
    for (int i = 0; i < cardTabs.size(); ++i) {
        _candidates.insert(make_pair(cardTabs.at(i), new (nothrow) Candidate()));
    }
    
    const auto& found = DeckManager::getInstance()->getFoundCards();
    for (auto it = begin(found); it != end(found); ++it) {
        // TODO: remove test code
        for (auto iter = begin(_candidates); iter != end(_candidates); ++iter) {
            auto candidate(iter->second);
            auto data(*it);
            candidate->foundCards.push_back(new (nothrow) CardSimpleData(data));
            candidate->unfoundCards.push_back(data->getName());
        }
    }
}

void BattleDeckLayer::clearCandidates()
{
    for (auto iter = begin(_candidates); iter != end(_candidates); ++iter) {
        CC_SAFE_DELETE(iter->second);
    }
    
    _candidates.clear();
}

void BattleDeckLayer::saveThisDeck()
{
    
}

void BattleDeckLayer::loadDeck(int idx)
{
    if (_thisDeckIdx != idx) {
        _thisDeckIdx = idx;
        
        // tab buttons
        for (auto iter = begin(_deckTabButtons); iter != end(_deckTabButtons); ++iter) {
            iter->second->setEnabled(idx != iter->first);
        }
    }
}

void BattleDeckLayer::setDefaultDeck()
{
    
}

void BattleDeckLayer::setCardType(DeckTabType type)
{
    if (_thisCardType != type) {
        _thisCardType = type;
        
        for (int i = 0; i < _candidateCards.size(); ++i) {
            auto node(_candidateCards.at(i));
            node->removeFromParent();
        }
        
        _candidateCards.clear();
        
        const auto info(_candidates.at(type));
        const auto& found(info->foundCards);
        const auto& unfound(info->unfoundCards);
        
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
            _candidateCards.push_back(card);
            
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
                (dynamic_cast<BattleDeckCard*>(node))->setIsHero(true);
                _scrollView->addChild(node);
                _candidateCards.push_back(node);
                
                const auto& point = getPosition(i / candidateColumn, i % candidateColumn);
                node->setPosition(Point(point.x, h2 + unfoundSpaceBottom - point.y));
            }
        }
        
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
            return LocalHelper::getString("ui_card_tab_all");
        case DeckTabType::Heroes:
            return LocalHelper::getString("ui_card_tab_hero");
        case DeckTabType::Soldiers:
            return LocalHelper::getString("ui_card_tab_soldier");
        case DeckTabType::Spells:
            return LocalHelper::getString("ui_card_tab_spell");
        default:
            return "";
    }
}
