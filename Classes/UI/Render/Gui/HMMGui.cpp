//
//  HMMGui.cpp
//  Underworld_Client
//
//  Created by Mac on 16/7/28.
//
//

#include "ui/CocosGUI.h"

#include "HMMGui.h"
#include "Game.h"
#include "World.h"
#include "TechTree.h"
#include "GameModeHMM.h"
#include "CocosUtils.h"
#include "Unit.h"
#include "BattleSmallResourceNode.h"
#include "DataManager.h"
#include "CardConfigData.h"
#include "GameModeHMM.h"
#include "HMMDeckRender.h"

namespace UnderWorld{ namespace Core{
    
static cocos2d::ProgressTimer* createProgressTimer(const string& file) {
    cocos2d::Sprite* s = cocos2d::Sprite::create(file);
    cocos2d::ProgressTimer* pt = cocos2d::ProgressTimer::create(s);
    pt->setType(cocos2d::ProgressTimer::Type::BAR);
    pt->setBarChangeRate(cocos2d::Vec2(1.0f, 0.0f));
    pt->setMidpoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    
    return pt;
}
    
const int HMMGui::COVER_UI_ZORDER = 1;
const int HMMGui::DECK_UI_ZORDER = 2;

const int HMMGui::INVALID_FACTION_INDEX = -1;
const int HMMGui::ALERT_REMAIN_TIME_IN_SECOND = 180;
const cocos2d::Color4B HMMGui::ALERT_REMAIN_TIME_COLOR = cocos2d::Color4B::RED;
const cocos2d::Color4B HMMGui::NORMAL_REMAIN_TIME_COLOR = cocos2d::Color4B::WHITE;
    
HMMGui::HMMGui()
: _thisFactionIndex(INVALID_FACTION_INDEX)
, _enemyFactionIndex(INVALID_FACTION_INDEX)
, _gameDuration(0)
, _thisCore(nullptr)
, _enemyCore(nullptr)
, _deckRender(nullptr)
, _gameOver(false)
, _guiView(nullptr)
, _timeLabel(nullptr)
, _thisHpProgressView(nullptr)
, _thisHpLabel(nullptr)
, _enemyHpProgressView(nullptr)
, _enemyHpLabel(nullptr)
, _game(nullptr)
, _commander(nullptr)
, _worldRender(nullptr) {
}
    
HMMGui::~HMMGui() {
    CC_SAFE_RELEASE(_guiView);
}

void HMMGui::init(const Game *game, Commander *commander, WorldRender *worldRender) {
    /**1. init data */
    _thisFactionIndex = game->getThisFactionIndex();
    _enemyFactionIndex = INVALID_FACTION_INDEX;
    for (int i = 0; i < game->getWorld()->getFactionCount(); ++i) {
        if (_thisFactionIndex != i) {
            _enemyFactionIndex = i;
            break;
        }
    }
    _gameDuration = game->getGameDuration();
    
    /**2. init ref */
    _game = game;
    _commander = commander;
    _worldRender = worldRender;
    
    /**3. init instance */
    _thisCore = nullptr;
    _enemyCore = nullptr;
    if (game->getWorld()->getFaction(_thisFactionIndex)) {
        _thisCore = game->getWorld()->getFaction(_thisFactionIndex)->findFirstUnitByClass(kUnitClass_Core);
    }
    if (_enemyFactionIndex != INVALID_FACTION_INDEX
        && game->getWorld()->getFaction(_enemyFactionIndex)) {
        _enemyCore = game->getWorld()->getFaction(_enemyFactionIndex)->findFirstUnitByClass(kUnitClass_Core);
    }
    _gameOver = false;
    
    /**4. init cocos */
    static const float ceilOffset(5.0f);
    
    //4a. clean up
    const cocos2d::Size& winSize = cocos2d::Director::getInstance()->getWinSize();
    if (!_guiView) {
        _guiView = cocos2d::LayerColor::create();
        _guiView->retain();
    }
    _guiView->removeAllChildren();
    _thisHpLabel = nullptr;
    _thisHpProgressView = nullptr;
    _enemyHpLabel = nullptr;
    _enemyHpProgressView = nullptr;
    _timeLabel = nullptr;
    
    //4b. faction info
    initFactionInfo(_game->getWorld()->getFaction(_thisFactionIndex));
    if (_enemyFactionIndex != INVALID_FACTION_INDEX) {
        initFactionInfo(_game->getWorld()->getFaction(_enemyFactionIndex));
    }

    //4c. time label
    {
        cocos2d::Sprite* sprite = cocos2d::Sprite::create("GameImages/test/ui_time.png");
        _guiView->addChild(sprite, COVER_UI_ZORDER);
        
        const cocos2d::Size& size = sprite->getContentSize();
        sprite->setPosition(cocos2d::Vec2(winSize.width / 2, winSize.height - size.height / 2 - ceilOffset));
        
        _timeLabel = CocosUtils::createLabel("", 24, DEFAULT_NUMBER_FONT);
        _timeLabel->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_LEFT);
        _timeLabel->setPosition(cocos2d::Vec2(size.width * 0.45f, size.height / 2));
        sprite->addChild(_timeLabel);
    }
    
    
    //4d. deck
    const HMMDeck* thisDeck = nullptr;
    const GameModeHMM* gameMode = dynamic_cast<const GameModeHMM*>(game->getGameMode());
    if (gameMode && _thisFactionIndex >= 0) thisDeck = gameMode->getDeck(_thisFactionIndex);
    if (!_deckRender) {
        _deckRender = HMMDeckRender::create(thisDeck, commander, game, worldRender);
    } else {
        _deckRender->init(thisDeck, commander, game, worldRender);
    }
    _guiView->addChild(_deckRender->getDeckView(), DECK_UI_ZORDER);
    
}
    
void HMMGui::render(const Game *game) {
    if (!game) return;
    
    //1. update time
    int timeInMicrosec = GameConstants::frame2Microsecond(game->getFrame());
    int timeRemainInSec = GameConstants::microsecond2Sec(m_clampi(_gameDuration - timeInMicrosec, 0, _gameDuration));
    if (_timeLabel) {
        _timeLabel->setString(CocosUtils::getFormattedTime(timeRemainInSec));
        
        if (timeRemainInSec <= ALERT_REMAIN_TIME_IN_SECOND) {
            _timeLabel->setTextColor(ALERT_REMAIN_TIME_COLOR);
        } else {
            _timeLabel->setTextColor(NORMAL_REMAIN_TIME_COLOR);
        }
    }
    
    //2. update hp
    int thisHpPercentage = _thisCore ? (100 * _thisCore->getHp() / _thisCore->getUnitBase().getMaxHp()) : 0;
    int enemyHpPercentage = _enemyCore ? (100 * _enemyCore->getHp() / _enemyCore->getUnitBase().getMaxHp()) : 0;
    if (_thisHpProgressView) _thisHpProgressView->setPercentage(thisHpPercentage);
    if (_thisHpLabel) _thisHpLabel->setString(UnderWorldCoreUtils::to_string(m_clampi(thisHpPercentage, 0, 100)) + "%");
    if (_enemyHpProgressView) _enemyHpProgressView->setPercentage(enemyHpPercentage);
    if (_enemyHpLabel) _enemyHpLabel->setString(UnderWorldCoreUtils::to_string(m_clampi(enemyHpPercentage, 0, 100)) + "%");
    
    //3. update deck
    const HMMDeck* thisDeck = nullptr;
    const GameModeHMM* gameMode = dynamic_cast<const GameModeHMM*>(game->getGameMode());
    if (gameMode && _thisFactionIndex >= 0) thisDeck = gameMode->getDeck(_thisFactionIndex);
    if (_deckRender) _deckRender->render(thisDeck, game);
    
    //4. update gameover
    if (game->isGameOver() && !_gameOver) {
        //TODO: show game over
        _gameOver = true;
    }
}
    
void HMMGui::initFactionInfo(const Faction *faction) {
    if (!faction || !_guiView) return;
    
    bool left = (_thisFactionIndex == faction->getFactionIndex());
    
    const cocos2d::Size& winSize = cocos2d::Director::getInstance()->getWinSize();
    
    cocos2d::Sprite* nameBg = CocosUtils::createPureColorSprite(cocos2d::Size(163, 26), LAYER_MASK_COLOR);
    _guiView->addChild(nameBg, COVER_UI_ZORDER);
    
    const cocos2d::Size& nameBgSize = nameBg->getContentSize();
    static const float offsetX(192.0f);
    nameBg->setPosition(cocos2d::Vec2(winSize.width / 2 + offsetX * (left ? -1 : 1), winSize.height - nameBgSize.height / 2));
    
    const cocos2d::Vec2& position = nameBg->getPosition();
    
    // name
    {
        std::string playerName;
        cocos2d::Label* label = CocosUtils::createLabel(playerName, DEFAULT_FONT_SIZE);
        label->setPosition(cocos2d::Vec2(nameBgSize.width / 2, nameBgSize.height / 2));
        nameBg->addChild(label);
    }
    
    // progress
    {
        cocos2d::Sprite* sprite = cocos2d::Sprite::create(left ? "GameImages/test/ui_blood_left_bg.png" : "GameImages/test/ui_blood_right_bg.png");
        _guiView->addChild(sprite, COVER_UI_ZORDER);
        
        const cocos2d::Size& size = sprite->getContentSize();
        sprite->setPosition(position - cocos2d::Vec2(0, (nameBgSize.height + size.height) / 2));
        
        cocos2d::ProgressTimer* pt = createProgressTimer(left ? "GameImages/test/ui_blood_left.png" : "GameImages/test/ui_blood_right.png");
        pt->setPosition(cocos2d::Vec2(size.width / 2, size.height / 2));
        pt->setMidpoint(left ? cocos2d::Vec2::ANCHOR_BOTTOM_RIGHT : cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
        sprite->addChild(pt);
        
        cocos2d::Label* ptLabel = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        ptLabel->setPosition(pt->getPosition());
        sprite->addChild(ptLabel);
        
        if (left) {
            _thisHpProgressView = pt;
            _thisHpLabel = ptLabel;
        } else {
            _enemyHpProgressView = pt;
            _enemyHpLabel = ptLabel;
        }
    }
    
    // icon
    {
        cocos2d::Sprite* sprite = cocos2d::Sprite::create(left ? "GameImages/test/icon_zg.png" : "GameImages/test/icon_bc.png");
        _guiView->addChild(sprite, COVER_UI_ZORDER);
        
        const cocos2d::Size& size = sprite->getContentSize();
        sprite->setPosition(cocos2d::Vec2(position.x + (nameBgSize.width / 2 + size.width / 2) * (left ? 1 : -1), winSize.height - size.height / 2));
    }
}
    
    
}}