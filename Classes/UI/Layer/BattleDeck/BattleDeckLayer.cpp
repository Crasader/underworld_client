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
#include "DeckManager.h"
#include "TabButton.h"
#include "UniversalButton.h"

using namespace std;

static const Vec2 edge(15, 15);
static const vector<DeckTabType> cardTabs = {
    DeckTabType::All,
    DeckTabType::Heroes,
    DeckTabType::Soldiers,
    DeckTabType::Spells
};

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
,_thisDeckIdx(-1)
,_thisDeckData(nullptr)
,_thisCardType(DeckTabType::None) {}

BattleDeckLayer::~BattleDeckLayer()
{
    removeAllChildren();
}

void BattleDeckLayer::registerObserver(BattleDeckLayerObserver *observer)
{
    _observer = observer;
}

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
        loadDeck(DeckManager::getInstance()->getDefaultDeckId());
        
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

#pragma mark - public
void BattleDeckLayer::createLeftNode()
{
    if (_background) {
        static const Size subSize(366, 496);
        auto node = CocosUtils::createSubBackground(subSize);
        node->setPosition(edge.x + subSize.width / 2, edge.y + subSize.height / 2);
        _background->addChild(node);
        
        {
            static const Vec2 secondaryEdge(5, 5);
            const Size subBgSize(subSize.width - secondaryEdge.x * 2, 60);
            auto subBg = CocosUtils::createPureBar(subBgSize);
            subBg->setPosition(subSize.width / 2, subSize.height - (secondaryEdge.y + subBgSize.height / 2));
            node->addChild(subBg);
            
            static const float thirdEdgeX(5);
            auto label = CocosUtils::createLabel("Battle Deck", DEFAULT_FONT_SIZE);
            label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            label->setPosition(thirdEdgeX, subBgSize.height / 2);
            subBg->addChild(label);
            
            float x(thirdEdgeX);
            for (int i = 0; i < DeckManager::MaxCount; ++i) {
                static const auto normal(CocosUtils::getResourcePath("button_lanse_2.png"));
                static const auto selected(CocosUtils::getResourcePath("button_lvse_3.png"));
                const auto idx(DeckManager::MaxCount - i);
                const string title = StringUtils::format("%d", idx);
                auto button = TabButton::create(title, normal, selected, [this, idx](Ref*) {
                    loadDeck(idx);
                });
                subBg->addChild(button);
                _deckTabButtons.push_back(button);
                
                const auto& size = button->getContentSize();
                button->setPosition(subBgSize.width - (x + size.width / 2), subBgSize.height / 2);
                
                static const float spaceX(10);
                x += size.width + spaceX;
            }
        }
    }
}

void BattleDeckLayer::createRightNode()
{
    if (_background) {
        const auto& size(_background->getContentSize());
        static const Size subSize(612, 496);
        auto node = CocosUtils::createSubBackground(subSize);
        node->setPosition(size.width - (edge.x + subSize.width / 2), edge.y + subSize.height / 2);
        _background->addChild(node);
        
        static const Vec2 secondaryEdge(5, 5);
        const Size subBgSize(subSize.width - secondaryEdge.x * 2, 92);
        auto subBg = CocosUtils::createPureBar(subBgSize);
        subBg->setPosition(subSize.width / 2, subSize.height - (secondaryEdge.y + subBgSize.height / 2));
        node->addChild(subBg);
        
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
                auto button = TabButton::create(getCardTabName(type), normal, selected, [this](Ref*) {
                    
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
    }
}

void BattleDeckLayer::updateCardsCount(int count)
{
    if (_cardsCountLabel) {
        _cardsCountLabel->setString(LocalHelper::getString("ui_card_progress") + StringUtils::format("%d/%d", count, 50));
    }
}

#pragma mark - functions
void BattleDeckLayer::saveThisDeck()
{
    
}

void BattleDeckLayer::loadDeck(int idx)
{
    if (_thisDeckIdx != idx) {
        _thisDeckIdx = idx;
    }
}

void BattleDeckLayer::setDefaultDeck()
{
    
}

void BattleDeckLayer::setCardType(DeckTabType type)
{
    if (_thisCardType != type) {
        _thisCardType = type;
        
        
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
