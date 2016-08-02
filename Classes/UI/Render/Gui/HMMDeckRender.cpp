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

namespace UnderWorld{ namespace Core{
    
const int HMMDeckRender::BACKGROUND_ZORDER = 1;
const int HMMDeckRender::RESOURCE_ZORDER = 2;
const int HMMDeckRender::CARD_ZORDER = 3;
const std::string HMMDeckRender::BATTLE_RESOURCE_NAME = RES_NAME_WOOD;
    
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
    
    static const float card_region_padding_horizontal(10.f);
    static const float card_region_padding_bottom(5.f);
    static const float card_region_padding_top(50.f);
    static const float card_interval_horizontal(8.f);
    static const float resource_interval_horizontal(1.f);

    
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
    _cardViews.clear();
    _resourceViews.clear();
    
    // 3b. card views
    for (int i = 0; i < _cardRenders.size(); ++i) {
        _cardViews.push_back(_cardRenders[i]->getCardView());
    }
    
    // 3d. contentSize
    float x = 0.f, y = 0.f;
    x += card_region_padding_horizontal * 2;
    if (_cardViews.size() > 0) {
        x += (_cardViews.size() - 1) * card_interval_horizontal;
    }
    for (int i = 0; i < _cardViews.size(); ++i) {
        if (_cardViews[i]) {
            x += _cardViews[i]->getContentSize().width;
            if (_cardViews[i]->getContentSize().height > y) {
                y = _cardViews[i]->getContentSize().height;
            }
        }
    }
    y += card_region_padding_bottom + card_region_padding_top;
    const cocos2d::Size contentSize(x, y);
    _cardRegion->setContentSize(contentSize);
    
    // 3e. background
    cocos2d::Rect rect(0, 0, 91, 157);
    static const float capInsets(18.0f);
    cocos2d::Rect capInsetsRect(capInsets, capInsets, rect.size.width - capInsets * 2, rect.size.height - capInsets * 2);
    _background = cocos2d::ui::Scale9Sprite::create("GameImages/test/ui_black_13.png", rect, capInsetsRect);
    _background->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    _background->setContentSize(contentSize);
    _cardRegion->addChild(_background, BACKGROUND_ZORDER);
    
    // 3f. cardViews
    float offestX = card_region_padding_horizontal;
    for (int i = 0; i < _cardViews.size(); ++i) {
        if (_cardViews[i]) {
            _cardViews[i]->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
            _cardViews[i]->setPosition(offestX, card_region_padding_bottom);
            _cardRegion->addChild(_cardViews[i], CARD_ZORDER);
            offestX += _cardViews[i]->getContentSize().width;
        }
        
        offestX += card_interval_horizontal;
    }

    // 3g. res progress
    cocos2d::Size progressSize(cocos2d::Size::ZERO);
    const ResourceType* rt = game ? game->getWorld()->getTechTree()->findResourceTypeByName(BATTLE_RESOURCE_NAME) : nullptr;
    if (rt) {
        int max = GameConstants::microres2Res(rt->getMax());
        
        for (int i = 0; i < max; ++i) {
            cocos2d::Sprite* s = cocos2d::Sprite::create("GameImages/test/ui_blood_9.png");
            cocos2d::ProgressTimer* pt = cocos2d::ProgressTimer::create(s);
            pt->setType(cocos2d::ProgressTimer::Type::BAR);
            pt->setBarChangeRate(cocos2d::Vec2(1.0f, 0.0f));
            pt->setMidpoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
            pt->setPercentage(100);
            pt->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
            _cardRegion->addChild(pt);
            
            _resourceViews.push_back(pt);
            
            if (progressSize.width == 0) {
                progressSize = pt->getContentSize();
            }
        }
        
        for (int i = 0; i < _resourceViews.size(); ++i) {
            auto pt = _resourceViews[i];
            if (pt) {
                pt->setPosition(card_region_padding_horizontal
                        + (progressSize.width + resource_interval_horizontal) * i,
                    contentSize.height - card_region_padding_top);
            }
        }
    }
    
    // 3h. register event
    auto* eventListener = cocos2d::EventListenerTouchOneByOne::create();
    eventListener->setSwallowTouches(true);
    eventListener->onTouchBegan = CC_CALLBACK_2(HMMDeckRender::onTouchBegan, this);
    eventListener->onTouchMoved = CC_CALLBACK_2(HMMDeckRender::onTouchMoved, this);
    eventListener->onTouchEnded = CC_CALLBACK_2(HMMDeckRender::onTouchEnded, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);
    
    /** init render */
    render(deck, game);
    
    return true;
}
    
void HMMDeckRender::render(const HMMDeck* deck, const Game* game) {
    if (!deck || !game) return;
    
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
        updateBattleResource(r->getBalanceMicro());
    }
}
    
void HMMDeckRender::updateBattleResource(microres_t amount) {
    microres_t max = GameConstants::res2Microres((int)_resourceViews.size());
    amount = m_clampi(amount, 0, max);
    for (int i = 0; i < _resourceViews.size(); ++i) {
        if (!_resourceViews[i]) continue;
        
        if (GameConstants::res2Microres(i) <= amount) {
            _resourceViews[i]->setPercentage(100.f);
        } else {
            _resourceViews[i]->setPercentage(0.f);
        }
    }
}

bool HMMDeckRender::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event) {
    
    return false;
}

void HMMDeckRender::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event) {
    
}

void HMMDeckRender::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event) {
    
}
    
HMMDeckRender::HMMDeckRender()
: _background(nullptr)
, _cardRegion(nullptr)
, _commander(nullptr)
, _worldRender(nullptr){
}

}}
