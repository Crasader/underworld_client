//
//  HMMDeckRender.cpp
//  Underworld_Client
//
//  Created by wenchengye on 16/7/30.
//
//

#include "HMMDeckRender.h"
#include "Global.h"
#include "HMMCardRender.h"
#include "CocosGlobal.h"
#include "GameModeHMM.h"
#include "HMMCardView.h"
#include "Game.h"
#include "World.h"
#include "TechTree.h"
#include "Commander.h"
#include "WorldRender.h"
#include "CocosUtils.h"

namespace UnderWorld{ namespace Core{
    
const int HMMDeckRender::BACKGROUND_ZORDER = 1;
const int HMMDeckRender::RESOURCE_ZORDER = 2;
const int HMMDeckRender::CARD_ZORDER = 3;
const std::string HMMDeckRender::BATTLE_RESOURCE_NAME = RES_NAME_WOOD;
const float HMMDeckRender::TOUCH_MOVE_THRESHOLD = 20.f;
    
HMMDeckRender* HMMDeckRender::create(const HMMDeck* deck, Commander* commander,
    const Game* game, WorldRender* worldRender) {
    HMMDeckRender* ret = new (std::nothrow) HMMDeckRender();
    if (ret && ret->init(deck, commander, game, worldRender)) {
        ret->autorelease();
    } else {
        CC_SAFE_DELETE(ret);
    }
    return ret;
}
    
HMMDeckRender::~HMMDeckRender() {
    for (int i = 0; i < _cardRenders.size(); ++i) {
        M_SAFE_DELETE(_cardRenders[i]);
    }
    _cardRenders.clear();
    
}

bool HMMDeckRender::init(const HMMDeck* deck, Commander* commander,
    const Game* game, WorldRender* worldRender) {
    if (!LayerColor::initWithColor(LAYER_DEFAULT_COLOR)) return false;
    
    static const float card_region_padding(10.f);
    static const float card_interval_horizontal(8.f);
    static const float resource_2_card_margin(8.f);
    static const float card_region_height(124.f);
    
    /**1. init ref */
    _commander = commander;
    _worldRender = worldRender;
    
    /**2. init instance */
    for (int i = 0; i < _cardRenders.size(); ++i) {
        M_SAFE_DELETE(_cardRenders[i]);
    }
    _cardRenders.clear();
    for (int i = 0; deck && i < deck->getHandCount(); ++i) {
        _cardRenders.push_back(new HMMCardRender());
        _cardRenders.back()->init(deck->getHandCard(i));
    }
    
    /**3. init cocos */
    // 3a. clean up
    if (!_cardRegion) {
        _cardRegion = cocos2d::Node::create();
        _cardRegion->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_BOTTOM);
        _cardRegion->setPosition(cocos2d::Vec2(this->getContentSize().width / 2, 0));
        this->addChild(_cardRegion);
    }
    _cardRegion->removeAllChildren();
    _background = nullptr;
    _resourceBackground = nullptr;
    _resourceProgress1 = nullptr;
    _resourceProgress2 = nullptr;
    _resourceLabel = nullptr;
    _cardViews.clear();
    
    // 3b. card views
    for (int i = 0; i < _cardRenders.size(); ++i) {
        _cardViews.push_back(_cardRenders[i]->getCardView());
    }
    
    // 3c. res progress views
    _resourceBackground = cocos2d::Sprite::create("GameImages/battle_ui/ui_resource_progress2_bg.png");
    cocos2d::Size resourceViewSize = _resourceBackground->getContentSize();
    
    _resourceProgress2 = cocos2d::ProgressTimer::create(cocos2d::Sprite::create("GameImages/battle_ui/ui_resource_progress2_fg.png"));
    if (_resourceProgress2) {
        _resourceProgress2->setType(cocos2d::ProgressTimer::Type::RADIAL);
        _resourceProgress2->setMidpoint(cocos2d::Vec2::ANCHOR_MIDDLE);
        _resourceProgress2->setReverseDirection(true);
        _resourceProgress2->setPosition(resourceViewSize.width / 2, resourceViewSize.height / 2);
        _resourceBackground->addChild(_resourceProgress2);
    }
    
    cocos2d::Node* resourceProgress1Bg = cocos2d::Sprite::create("GameImages/battle_ui/ui_resource_progress1_bg.png");
    if (resourceProgress1Bg) {
        resourceProgress1Bg->setPosition(resourceViewSize.width / 2, resourceViewSize.height / 2);
        _resourceBackground->addChild(resourceProgress1Bg);
    }
    
    _resourceProgress1 = cocos2d::ProgressTimer::create(cocos2d::Sprite::create("GameImages/battle_ui/ui_resource_progress1_fg.png"));
    if (_resourceProgress1) {
        _resourceProgress1->setType(cocos2d::ProgressTimer::Type::BAR);
        _resourceProgress1->setBarChangeRate(cocos2d::Vec2(0, 1.f));
        _resourceProgress1->setMidpoint(cocos2d::Vec2::ANCHOR_MIDDLE_BOTTOM);
        _resourceProgress1->setPosition(resourceViewSize.width / 2, resourceViewSize.height / 2);
        _resourceBackground->addChild(_resourceProgress1);
    }
    
    _resourceLabel = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
    if (_resourceLabel) {
        _resourceLabel->setPosition(resourceViewSize.width / 2, resourceViewSize.height / 2);
        _resourceBackground->addChild(_resourceLabel);
    }
    
    // 3d. contentSize
    float x = 0.f, y = card_region_height;
    x += card_region_padding * 2;
    x += resourceViewSize.width;
    x += resource_2_card_margin;
    if (_cardViews.size() > 0) {
        x += (_cardViews.size() - 1) * card_interval_horizontal;
    }
    for (int i = 0; i < _cardViews.size(); ++i) {
        if (_cardViews[i]) x+= _cardViews[i]->getContentSize().width;
    }
    
    const cocos2d::Size contentSize(x, y);
    _cardRegion->setContentSize(contentSize);
    
    // 3e. background
    cocos2d::Rect rect(0, 0, 216, card_region_height);
    cocos2d::Rect capInsetsRect(108, 62, 1, 1);
    _background = cocos2d::ui::Scale9Sprite::create("GameImages/battle_ui/ui_deck_bg.png", rect, capInsetsRect);
    _background->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    _background->setContentSize(contentSize);
    _cardRegion->addChild(_background, BACKGROUND_ZORDER);
    
    // 3f. add card views
    float offestX = card_region_padding + resourceViewSize.width + resource_2_card_margin;
    for (int i = 0; i < _cardViews.size(); ++i) {
        if (_cardViews[i]) {
            _cardViews[i]->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
            _cardViews[i]->setPosition(offestX, card_region_padding);
            _cardRegion->addChild(_cardViews[i], CARD_ZORDER);
            offestX += _cardViews[i]->getContentSize().width;
        }
        
        offestX += card_interval_horizontal;
    }
    
    // 3g. add resource views
    _resourceBackground->setPosition(card_region_padding + resourceViewSize.width / 2, card_region_height / 2);
    _cardRegion->addChild(_resourceBackground, RESOURCE_ZORDER);
    
    // 3h. register event
    auto* eventListener = cocos2d::EventListenerTouchOneByOne::create();
    eventListener->setSwallowTouches(false);
    eventListener->onTouchBegan = CC_CALLBACK_2(HMMDeckRender::onTouchBegan, this);
    eventListener->onTouchMoved = CC_CALLBACK_2(HMMDeckRender::onTouchMoved, this);
    eventListener->onTouchEnded = CC_CALLBACK_2(HMMDeckRender::onTouchEnded, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);
    
    /**4. init status */
    resetTouchEventStatus();
    _selectedCardIndex = CARD_INDEX_INVALID;
    
    /**5. init render */
    render(deck, game);
    
    return true;
}
    
void HMMDeckRender::render(const HMMDeck* deck, const Game* game) {
    if (!deck || !game) return;
    
    _deck = deck;
    _game = game;
    
    /** 1. render cards */
    for (int i = 0; i < deck->getHandCount() && i < _cardRenders.size(); ++i) {
        if (_cardRenders[i]) _cardRenders[i]->render(deck->getHandCard(i));
    }
    
    /** 2. update battle resource */
    const Resource* r = nullptr;
    if (game->getWorld()->getFaction(deck->getFactionIndex())) {
        r = game->getWorld()->getFaction(deck->getFactionIndex())->getResource(BATTLE_RESOURCE_NAME);
    }
    if (r) {
        updateBattleResource(r->getBalanceMicro(), r->getResourceType()->getMax());
    }
}
    
void HMMDeckRender::updateBattleResource(microres_t amount, microres_t max) {
    int res = GameConstants::microres2Res(amount);
    int resRemind = amount % GameConstants::MICRORES_PER_RES;
    
    if (_resourceLabel) {
        _resourceLabel->setString(UnderWorldCoreUtils::to_string(res));
    }
    
    if (_resourceProgress1) {
        _resourceProgress1->setPercentage(amount * 100 / max);
    }
    
    if (_resourceProgress2) {
        _resourceProgress2->setPercentage(resRemind * 100 / GameConstants::MICRORES_PER_RES);
    }
    
}

bool HMMDeckRender::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event) {
    bool ret = false;
    
    resetTouchEventStatus();
    
    _touchBeginPos = touch->getLocation();
    _pressingCardIndex = calculatePositionOnWhichCard(_cardRegion->convertToNodeSpace(this->convertToWorldSpace(_touchBeginPos)));
    if (_pressingCardIndex != CARD_INDEX_INVALID) {
        _pressingCardOriginPos = _cardViews[_pressingCardIndex]->getPosition();
        
        ret = true;
    } else if (_selectedCardIndex != CARD_INDEX_INVALID && !calculatePositionInCardRegion(_touchBeginPos)) {
        _placingCard = true;
        ret = true;
    }
    

    return ret;
}
    
void HMMDeckRender::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event) {
    if (_placingCard
        && _touchBeginPos.distance(touch->getLocation()) > TOUCH_MOVE_THRESHOLD) {
        _placingCard = false;
    }
    
}

void HMMDeckRender::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event) {
    
    if (_pressingCardIndex != CARD_INDEX_INVALID) {
        if (_cardViews[_pressingCardIndex]->getBoundingBox().containsPoint(_cardRegion->convertToNodeSpace(this->convertToWorldSpace(touch->getLocation())))) {
            if (_selectedCardIndex == _pressingCardIndex) {
                selectCard(CARD_INDEX_INVALID);
            } else {
                selectCard(_pressingCardIndex);
            }
        }
    } else if (_placingCard && _selectedCardIndex != CARD_INDEX_INVALID && !calculatePositionInCardRegion(touch->getLocation())) {
        try2UseCard(_selectedCardIndex, touch->getLocation());
    }
}

HMMDeckRender::HMMDeckRender()
: _background(nullptr)
, _cardRegion(nullptr)
, _pressingCardIndex(CARD_INDEX_INVALID)
, _draggingCard(false)
, _placingCard(false)
, _commander(nullptr)
, _worldRender(nullptr)
, _deck(nullptr)
, _game(nullptr){
}

void HMMDeckRender::resetTouchEventStatus() {
    _touchBeginPos.set(0.f, 0.f);
    _pressingCardIndex = CARD_INDEX_INVALID;
    _pressingCardOriginPos.set(0.f, 0.f);
    _draggingCard = false;
    _placingCard = false;
}
    
int HMMDeckRender::calculatePositionOnWhichCard(const cocos2d::Vec2& pos) {
    int ret = CARD_INDEX_INVALID;
    for (int i = 0; i < _cardViews.size(); ++i) {
        if (_cardViews[i] && _cardViews[i]->getBoundingBox().containsPoint(pos)) {
            ret = i;
            break;
        }
    }
    return ret;
}
    
bool HMMDeckRender::calculatePositionInCardRegion(const cocos2d::Vec2& pos) {
    return _cardRegion && _cardRegion->getBoundingBox().containsPoint(pos);
}
    
void HMMDeckRender::selectCard(int index) {
    if (_selectedCardIndex == index) return;
    
    if (_selectedCardIndex != CARD_INDEX_INVALID) {
        _cardViews[_selectedCardIndex]->setSelected(false);
    }
    _selectedCardIndex = index;
    if (_selectedCardIndex != CARD_INDEX_INVALID) {
        _cardViews[_selectedCardIndex]->setSelected(true);
    }
    
    if (!_worldRender) return;
    
    if (_selectedCardIndex == CARD_INDEX_INVALID) {
        _worldRender->hideHMMCardRegionTips();
    } else {
        if (!_deck) return;
        const HMMCard* card = _deck->getHandCard(index);
        if (!card) return;
        
        _worldRender->showHMMCardRegionTips(_deck, card);
    }
}

void HMMDeckRender::try2UseCard(int cardIndex, const cocos2d::Vec2 &pos) {
    if (!_worldRender || !_deck || !_game || !_commander) return;
    
    // check card instacne
    const HMMCard* card = _deck->getHandCard(cardIndex);
    if (!card) return;
    
    // check card ready
    if (!card->isCardReady()) return;
    
    // vaildate core pos
    Coordinate32 corePos = _worldRender->cocosPoint2WorldCoordinate(_worldRender->getWorldContainer()->convertToNodeSpace(this->convertToWorldSpace(pos)));
    corePos.x = m_clampi(corePos.x, 0, _game->getWorld()->getMap()->getMapElementWidth());
    corePos.y = m_clampi(corePos.y, 0, _game->getWorld()->getMap()->getMapElementHeight());
    if (card->getCardType()->getCardClass() == kHMMCardClass_Summon
        || card->getCardType()->getCardClass() == kHMMCardClass_Tower) {
        const Rect32& rect = card->getCardType()->getCardClass() == kHMMCardClass_Tower ? _deck->getTowerRegion() : _deck->getSummonRegion();
        
        corePos.x = m_clampi(corePos.x, rect.getMinX(), rect.getMaxX());
        corePos.y = m_clampi(corePos.y, rect.getMinY(), rect.getMaxY());
    }
    
    // give command
    _commander->addCommandFromLocal(new OutsideHMMCommand(cardIndex, _deck->getFactionIndex(), corePos));
}
    
    
}}
