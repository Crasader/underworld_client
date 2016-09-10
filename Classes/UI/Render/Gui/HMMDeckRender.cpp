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
    
const std::string HMMDeckRender::LONG_PRESS_CARD_SCHEDULE_KEY("hmm_deck_render_card_long_press_schedule_key");
const float HMMDeckRender::LONG_PRESS_CARD_SCHEDULE_DELAY = 0.2f;
const int HMMDeckRender::LONG_PRESS_ACTION_TAG = 0;
const float HMMDeckRender::LONG_PRESS_SCALE_DOWN_DURATION = 0.1f;
const float HMMDeckRender::LONG_PRESS_SCALE_DOWN = 0.9f;
const float HMMDeckRender::LONG_PRESS_SCALE_UP_DURATION = 0.2f;
const float HMMDeckRender::LONG_PRESS_SCALE_UP = 1.1f;
    
 const int HMMDeckRender::SELECT_ACTION_TAG = 1;
 const float HMMDeckRender::SELECT_ACTION_DURATION = 0.75f;
 const float HMMDeckRender::SELECT_MOVE_DISTANCE = 10.f;
 const float HMMDeckRender::SELECT_MOVE_WAVE_OFFSET = 3.f;
    
const float HMMDeckRender::ACTIVE_HERO_MOVE_DURATION = 0.5f;
const float HMMDeckRender::ACTIVE_HERO_FATE_DURATION = 0.8f;
const float HMMDeckRender::ACTIVE_HERO_MOVE_WAVE_OFFEST = 15.f;
    
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
    
    cancelLongPressSchedule();
}

bool HMMDeckRender::init(const HMMDeck* deck, Commander* commander,
    const Game* game, WorldRender* worldRender) {
    if (!LayerColor::initWithColor(LAYER_DEFAULT_COLOR)) return false;
    
    static const float card_region_padding_horizontal(12.f);
    static const float card_region_padding_vertical(10.f);
    static const float card_interval_horizontal(14.f);
    static const float resource_2_card_margin(8.f);
    static const float card_region_height(124.f);
    static const float hero_2_normal_margin(80.f);
    
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
    
    /**2.1 data */
    _summonRegion = deck->getSummonRegion();
    _towerRegion = deck->getTowerRegion();
    
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
    _cardOriginPos.clear();
    _cardSelectedPos.clear();
    int heroCardCount = 0;
    for (; heroCardCount < deck->getHandCount(); ++heroCardCount) {
        if (deck->getHandCard(heroCardCount)->getCardType()->getCardClass() != kHMMCardClass_Hero) break;
    }
    
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
    float x = 0.f, x_normal = 0.f, x_hero = 0.f, y = card_region_height;
    
    if (heroCardCount > 0) {
        x_hero += card_region_padding_horizontal * 2;
        x_hero += (heroCardCount - 1) * card_interval_horizontal;
        for (int i = 0; i < heroCardCount; ++i) {
            if (_cardViews[i]) x_hero+= _cardViews[i]->getContentSize().width;
        }
    }
    
    x_normal += card_region_padding_horizontal * 2;
    x_normal += resourceViewSize.width;
    x_normal += resource_2_card_margin;
    if (_cardViews.size() - heroCardCount > 0) {
        x_normal += (_cardViews.size() - heroCardCount - 1) * card_interval_horizontal;
    }
    for (int i = heroCardCount; i < _cardViews.size(); ++i) {
        if (_cardViews[i]) x_normal += _cardViews[i]->getContentSize().width;
    }
    
    x += x_normal;
    if (heroCardCount > 0) x += hero_2_normal_margin + x_hero;
    const cocos2d::Size contentSize(x, y);
    _cardRegion->setContentSize(contentSize);
    _heroRegionWidth = x_hero;
    
    // 3e. background
    cocos2d::Rect rect(0, 0, 216, 124);
    cocos2d::Rect capInsetsRect(108, 62, 1, 1);
    _background = cocos2d::ui::Scale9Sprite::create("GameImages/battle_ui/ui_deck_bg.png", rect, capInsetsRect);
    _background->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    _background->setContentSize(cocos2d::Size(x_normal, y));
    _background->setPosition(cocos2d::Vec2(x - x_normal, 0));
    _cardRegion->addChild(_background, BACKGROUND_ZORDER);
    
    // 3f. add card views
    float offsetX = 0.f;
    if (heroCardCount > 0) {
        offsetX += card_region_padding_horizontal;
        for (int i = 0; i < heroCardCount; ++i) {
            if (_cardViews[i]) {
                _cardViews[i]->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
                _cardViews[i]->setPosition(offsetX + _cardViews[i]->getContentSize().width / 2, card_region_padding_vertical + _cardViews[i]->getContentSize().height / 2);
                _cardRegion->addChild(_cardViews[i], CARD_ZORDER);
                offsetX += _cardViews[i]->getContentSize().width;
                offsetX += card_interval_horizontal;
            }
            
        }
        offsetX += hero_2_normal_margin;
    }
    offsetX += card_region_padding_horizontal + resourceViewSize.width + resource_2_card_margin;
    for (int i = heroCardCount; i < _cardViews.size(); ++i) {
        if (_cardViews[i]) {
            _cardViews[i]->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
            _cardViews[i]->setPosition(offsetX + _cardViews[i]->getContentSize().width / 2, card_region_padding_vertical + _cardViews[i]->getContentSize().height / 2);
            _cardRegion->addChild(_cardViews[i], CARD_ZORDER);
            offsetX += _cardViews[i]->getContentSize().width;
            offsetX += card_interval_horizontal;
        }
    }
    
    // 3g. add resource views
    _resourceBackground->setPosition(x - x_normal + card_region_padding_horizontal + resourceViewSize.width / 2, card_region_height / 2);
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
    for (int i = 0; i < _cardViews.size(); ++i) {
        _cardOriginPos.push_back(_cardViews[i]->getPosition());
        _cardSelectedPos.push_back(_cardViews[i]->getPosition() + cocos2d::Vec2(0.f, SELECT_MOVE_DISTANCE));
    }
    _activeHeroIndex = CARD_INDEX_INVALID;
    _lockDeck = false;
    
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
    
    /**3. update active hero */
    int activeHero = CARD_INDEX_INVALID;
    for (int i = 0; i < deck->getHandCount(); ++i) {
        const HMMCard* card = deck->getHandCard(i);
        if (card
            && card->getCardType()->getCardClass() == kHMMCardClass_Hero
            && card->getHeroCardState() == HMMCard::HeroCardState::Active) {
            activeHero = i;
            break;
        }
    }
    updateActiveHero(activeHero);
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
    
void HMMDeckRender::updateActiveHero(int index) {
    if (index == _activeHeroIndex) return;
    
    if (_activeHeroIndex != CARD_INDEX_INVALID) {
        // back
        
        // move
        cocos2d::Vec2 targetPos(_cardOriginPos[_activeHeroIndex].x, _cardViews[_activeHeroIndex]->getPosition().y);
        int direction = targetPos.x > _cardViews[_activeHeroIndex]->getPosition().x ? 1 : -1;
        
        cocos2d::Action* seq = cocos2d::Sequence::create(
            cocos2d::EaseSineOut::create(cocos2d::MoveTo::create(ACTIVE_HERO_MOVE_DURATION / 2, targetPos + cocos2d::Vec2(ACTIVE_HERO_MOVE_WAVE_OFFEST * direction, 0.f))),
            cocos2d::EaseSineOut::create(cocos2d::MoveTo::create(ACTIVE_HERO_MOVE_DURATION / 4, targetPos + cocos2d::Vec2(-1 * ACTIVE_HERO_MOVE_WAVE_OFFEST * direction, 0.f))),
            cocos2d::EaseSineOut::create(cocos2d::MoveTo::create(ACTIVE_HERO_MOVE_DURATION / 4, targetPos)),
            cocos2d::CallFunc::create([this](){ this->_lockDeck = false; }),
            NULL);
        
        _cardViews[_activeHeroIndex]->runAction(seq);
        
        // fade
        for (int i = 0; i < _cardViews.size(); ++i) {
            if (i == _activeHeroIndex || _cardViews[i]->getCardType()->getCardClass() != kHMMCardClass_Hero) continue;
            
            cocos2d::Action* fadeIn = cocos2d::Sequence::create(
                cocos2d::DelayTime::create(ACTIVE_HERO_MOVE_DURATION),
                cocos2d::FadeIn::create(ACTIVE_HERO_FATE_DURATION),
                nullptr
            );
            
            _cardViews[i]->runAction(fadeIn);
        }
        
        _lockDeck = true;
    } else {
        // forward
        
        // fade
        for (int i = 0; i < _cardViews.size(); ++i) {
            if (i == index || _cardViews[i]->getCardType()->getCardClass() != kHMMCardClass_Hero) continue;
            
            cocos2d::Action* fadeOut = cocos2d::FadeOut::create(ACTIVE_HERO_FATE_DURATION);
            _cardViews[i]->runAction(fadeOut);
        }
        
        // move
        cocos2d::Vec2 targetPos(_heroRegionWidth / 2, _cardViews[index]->getPosition().y);
        int direction = targetPos.x > _cardViews[index]->getPosition().x ? 1 : -1;
        
        cocos2d::Action* seq = cocos2d::Sequence::create(
            cocos2d::DelayTime::create(ACTIVE_HERO_FATE_DURATION),
            cocos2d::EaseSineOut::create(cocos2d::MoveTo::create(ACTIVE_HERO_MOVE_DURATION / 2, targetPos + cocos2d::Vec2(ACTIVE_HERO_MOVE_WAVE_OFFEST * direction, 0.f))),
            cocos2d::EaseSineOut::create(cocos2d::MoveTo::create(ACTIVE_HERO_MOVE_DURATION / 4, targetPos + cocos2d::Vec2(-1 * ACTIVE_HERO_MOVE_WAVE_OFFEST * direction, 0.f))),
            cocos2d::EaseSineOut::create(cocos2d::MoveTo::create(ACTIVE_HERO_MOVE_DURATION / 4, targetPos)),
            cocos2d::CallFunc::create([this](){ this->_lockDeck = false; }),
            NULL);
        _cardViews[index]->runAction(seq);
        
        _lockDeck = true;
    }
    _activeHeroIndex = index;
}
    
void HMMDeckRender::markObjectReleased() {
    _commander = nullptr;
    _deck = nullptr;
    _game = nullptr;
}

bool HMMDeckRender::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event) {
    bool ret = false;
    
    if (_lockDeck) return ret;
    
    resetTouchEventStatus();
    
    _touchBeginPos = touch->getLocation();
    _pressingCardIndex = calculatePositionOnWhichCard(_cardRegion->convertToNodeSpace(this->convertToWorldSpace(_touchBeginPos)));
    if (_pressingCardIndex != CARD_INDEX_INVALID) {
        _pressingCardOriginPos = _cardViews[_pressingCardIndex]->getPosition();
        startLongPressSchedule();
        
        ret = true;
    } else if (_selectedCardIndex != CARD_INDEX_INVALID && !calculatePositionInCardRegion(_touchBeginPos)) {
        placeCardTouch(_selectedCardIndex, _touchBeginPos);
        
        _placingCard = true;
        ret = true;
    }
    
    return ret;
}
    
void HMMDeckRender::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event) {
    if (_placingCard && _selectedCardIndex != CARD_INDEX_INVALID) {
        placeCardTouch(_selectedCardIndex, touch->getLocation());
    }
}

void HMMDeckRender::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event) {
    
    if (_pressingCardIndex != CARD_INDEX_INVALID) {
        if (_longPressing || _cardViews[_pressingCardIndex]->getBoundingBox().containsPoint(_cardRegion->convertToNodeSpace(this->convertToWorldSpace(touch->getLocation())))) {
            if (_selectedCardIndex == _pressingCardIndex) {
                selectCard(CARD_INDEX_INVALID);
            } else {
                selectCard(_pressingCardIndex);
            }
        }
        
        if (_longPressing) {
            longPressEnd();
        } else {
            cancelLongPressSchedule();
        }
        
    } else if (_placingCard && _selectedCardIndex != CARD_INDEX_INVALID) {
        placeCardTouchEnd(_selectedCardIndex, touch->getLocation());
    }
}

HMMDeckRender::HMMDeckRender()
: _background(nullptr)
, _resourceBackground(nullptr)
, _resourceProgress1(nullptr)
, _resourceProgress2(nullptr)
, _resourceLabel(nullptr)
, _cardRegion(nullptr)
, _heroRegionWidth(0.f)
, _pressingCardIndex(CARD_INDEX_INVALID)
, _draggingCard(false)
, _placingCard(false)
, _longPressing(false)
, _selectedCardIndex(CARD_INDEX_INVALID)
, _activeHeroIndex(CARD_INDEX_INVALID)
, _lockDeck(true)
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
    _longPressing = false;
}
    
int HMMDeckRender::calculatePositionOnWhichCard(const cocos2d::Vec2& pos) {
    int ret = CARD_INDEX_INVALID;
    for (int i = 0; i < _cardViews.size(); ++i) {
        if (_activeHeroIndex != CARD_INDEX_INVALID
            && _cardViews[i]->getCardType()->getCardClass() == kHMMCardClass_Hero
            && i != _activeHeroIndex) {
            continue;
        }
        
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
    
void HMMDeckRender::startLongPressSchedule() {
    this->schedule(std::bind(&HMMDeckRender::longPressBegin, this, std::placeholders::_1), 0.f, false, LONG_PRESS_CARD_SCHEDULE_DELAY, LONG_PRESS_CARD_SCHEDULE_KEY);
}
    
void HMMDeckRender::cancelLongPressSchedule() {
    this->unschedule(LONG_PRESS_CARD_SCHEDULE_KEY);
}
    
void HMMDeckRender::longPressBegin(float dt) {
    _longPressing = true;
    if (_pressingCardIndex != CARD_INDEX_INVALID) {
        _cardViews[_pressingCardIndex]->stopActionByTag(LONG_PRESS_ACTION_TAG);
        
        cocos2d::Action* scaleDown = cocos2d::EaseSineIn::create(cocos2d::ScaleTo::create(LONG_PRESS_SCALE_DOWN_DURATION, LONG_PRESS_SCALE_DOWN));
        scaleDown->setTag(LONG_PRESS_ACTION_TAG);
        
        _cardViews[_pressingCardIndex]->runAction(scaleDown);
    }
}
    
void HMMDeckRender::longPressEnd() {
    _longPressing = false;
    if (_pressingCardIndex != CARD_INDEX_INVALID) {
        _cardViews[_pressingCardIndex]->stopActionByTag(LONG_PRESS_ACTION_TAG);
        
        cocos2d::Action* scaleUp = cocos2d::Sequence::create(
            cocos2d::EaseSineIn::create(cocos2d::ScaleTo::create(LONG_PRESS_SCALE_UP_DURATION / 3 * 2, LONG_PRESS_SCALE_UP)),
            cocos2d::EaseSineIn::create(cocos2d::ScaleTo::create(LONG_PRESS_SCALE_UP_DURATION / 3, 1.f)),
            nullptr);
        scaleUp->setTag(LONG_PRESS_ACTION_TAG);
        
        _cardViews[_pressingCardIndex]->runAction(scaleUp);
    }
}
    
void HMMDeckRender::placeCardTouch(int index, const cocos2d::Vec2 &location) {
    if (calculatePositionInCardRegion(location)) {
        _worldRender->hideHMMCardPlaceTips();
    } else {
        const HMMCard* card = _deck->getHandCard(index);
        if (card) {
            Coordinate32 pos = transformAndValidateLoction2WorldCoordinate(location, card->getCardType());
            
            _worldRender->showHMMCardPlaceTips(card->getCardType(), pos, true);
        }
    }
}
    
void HMMDeckRender::placeCardTouchEnd(int index, const cocos2d::Vec2 &location) {
    _worldRender->hideHMMCardPlaceTips();
    
    if (calculatePositionInCardRegion(location)) return;
    
    try2UseCard(_selectedCardIndex, location);
}
    

    
void HMMDeckRender::selectCard(int index) {
    if (_selectedCardIndex == index) {
        playSelectAnim(_selectedCardIndex);
    } else {
        if (_selectedCardIndex != CARD_INDEX_INVALID) {
            _cardViews[_selectedCardIndex]->setSelected(false);
            playUnselectAnim(_selectedCardIndex);
        }
        _selectedCardIndex = index;
        if (_selectedCardIndex != CARD_INDEX_INVALID) {
            _cardViews[_selectedCardIndex]->setSelected(true);
            playSelectAnim(_selectedCardIndex);
        }
        
        if (!_worldRender) return;
        
        if (_selectedCardIndex == CARD_INDEX_INVALID) {
            _worldRender->hideHMMCardRegionTips();
        } else {
            _worldRender->showHMMCardRegionTips(
                _cardViews[_selectedCardIndex]->getCardType(),
                _summonRegion,
                _towerRegion);
        }
    }
}
    
void HMMDeckRender::playSelectAnim(int index) {
    _cardViews[index]->stopActionByTag(SELECT_ACTION_TAG);
    
    cocos2d::Vec2 targetPos(_cardViews[index]->getPosition().x, _cardSelectedPos[index].y);
    cocos2d::Action* moveUp = cocos2d::Sequence::create(
        cocos2d::EaseSineIn::create(cocos2d::MoveTo::create(SELECT_ACTION_DURATION / 4, targetPos + cocos2d::Vec2(0.f, SELECT_MOVE_WAVE_OFFSET))),
        cocos2d::EaseSineIn::create(cocos2d::MoveTo::create(SELECT_ACTION_DURATION / 4, targetPos + cocos2d::Vec2(0.f, SELECT_MOVE_WAVE_OFFSET / 2 * -1))),
        cocos2d::EaseSineIn::create(cocos2d::MoveTo::create(SELECT_ACTION_DURATION / 4, targetPos + cocos2d::Vec2(0.f, SELECT_MOVE_WAVE_OFFSET / 2))),
        cocos2d::EaseSineIn::create(cocos2d::MoveTo::create(SELECT_ACTION_DURATION / 4, targetPos))
        ,NULL);
    moveUp->setTag(SELECT_ACTION_TAG);
    
    _cardViews[index]->runAction(moveUp);
}
    
void HMMDeckRender::playUnselectAnim(int index) {
    _cardViews[index]->stopActionByTag(SELECT_ACTION_TAG);
    
    cocos2d::Vec2 targetPos(_cardViews[index]->getPosition().x, _cardOriginPos[index].y);
    cocos2d::Action* moveDown = cocos2d::Sequence::create(
        cocos2d::EaseSineIn::create(cocos2d::MoveTo::create(SELECT_ACTION_DURATION / 4, targetPos + cocos2d::Vec2(0.f, SELECT_MOVE_WAVE_OFFSET * -1))),
        cocos2d::EaseSineIn::create(cocos2d::MoveTo::create(SELECT_ACTION_DURATION / 4, targetPos + cocos2d::Vec2(0.f, SELECT_MOVE_WAVE_OFFSET / 2))),
        cocos2d::EaseSineIn::create(cocos2d::MoveTo::create(SELECT_ACTION_DURATION / 4, targetPos + cocos2d::Vec2(0.f, SELECT_MOVE_WAVE_OFFSET / 2 * -1))),
        cocos2d::EaseSineIn::create(cocos2d::MoveTo::create(SELECT_ACTION_DURATION / 4, targetPos))
        ,NULL);
    moveDown->setTag(SELECT_ACTION_TAG);
    
    _cardViews[index]->runAction(moveDown);
}

void HMMDeckRender::try2UseCard(int cardIndex, const cocos2d::Vec2 &pos) {
    if (!_worldRender || !_deck || !_game || !_commander) return;
    
    // check card instacne
    const HMMCard* card = _deck->getHandCard(cardIndex);
    if (!card) return;
    
    // check card ready
    if (!card->isCardReady()) return;
    
    // vaildate core pos
    Coordinate32 corePos = transformAndValidateLoction2WorldCoordinate(pos, card->getCardType());
    // give command
    _commander->addCommandFromLocal(new OutsideHMMCommand(cardIndex, _deck->getFactionIndex(), corePos));
}
    
Coordinate32 HMMDeckRender::transformAndValidateLoction2WorldCoordinate(const cocos2d::Vec2& location, const HMMCardType* cardType) {
    Coordinate32 ret = _worldRender->cocosPoint2WorldCoordinate(_worldRender->getWorldContainer()->convertToNodeSpace(this->convertToWorldSpace(location)));
    
    Rect32 legalRegion(0, 0,
        _game->getWorld()->getMap()->getMapElementWidth(),
        _game->getWorld()->getMap()->getMapElementHeight());
    
    if (cardType) {
        if (cardType->getCardClass() == kHMMCardClass_Tower) {
            legalRegion = _deck->getTowerRegion();
        } else if (cardType->getCardClass() == kHMMCardClass_Hero
            || cardType->getCardClass() == kHMMCardClass_Summon) {
            legalRegion = _deck->getSummonRegion();
        } else if (cardType->getCardClass() == kHMMCardClass_Spell) {
            legalRegion.setRect(0, 0,
                _game->getWorld()->getMap()->getMapElementWidth(),
                _game->getWorld()->getMap()->getMapElementHeight());
        }
        
        if (cardType->getCardClass() == kHMMCardClass_Tower
            || cardType->getCardClass() == kHMMCardClass_Summon
            || cardType->getCardClass() == kHMMCardClass_Hero) {
            const UnitType* ut = _game->getWorld()->getTechTree()->findUnitTypeById(cardType->getUnitId());
            
            if (ut) {
                int size = ut->getSize();
                legalRegion.setRect(legalRegion.getMinX(), legalRegion.getMinY(), M_MAX(legalRegion._width - size, 0), M_MAX(legalRegion._height - size, 0));
            }
        }
    }
    
    ret.x = m_clampi(ret.x, legalRegion.getMinX(), legalRegion.getMaxX());
    ret.y = m_clampi(ret.y, legalRegion.getMinY(), legalRegion.getMaxY());
    
    
    return ret;
}
    
    
}}
