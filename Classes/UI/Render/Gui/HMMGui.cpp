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
#include "TechTree.h"
#include "GameModeHMM.h"
#include "Utils.h"
#include "CocosUtils.h"
#include "Unit.h"
#include "BattleSmallResourceNode.h"
#include "DataManager.h"
#include "CardConfigData.h"
#include "GameModeHMM.h"
#include "HMMDeckRender.h"
#include "MessageBoxLayer.h"
#include "GameManager.h"
#include "MainScene.h"

namespace UnderWorld{ namespace Core{
    
static cocos2d::ProgressTimer* createProgressTimer(const string& file) {
    cocos2d::Sprite* s = cocos2d::Sprite::create(file);
    cocos2d::ProgressTimer* pt = cocos2d::ProgressTimer::create(s);
    pt->setType(cocos2d::ProgressTimer::Type::BAR);
    pt->setBarChangeRate(cocos2d::Vec2(1.0f, 0.0f));
    pt->setMidpoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    
    return pt;
}
    
const int HMMGui::COVER_UI_ZORDER = 2;
const int HMMGui::DECK_UI_ZORDER = 1;

const int HMMGui::INVALID_FACTION_INDEX = -1;
const int HMMGui::ALERT_REMAIN_TIME_IN_SECOND = 180;
const cocos2d::Color4B HMMGui::ALERT_REMAIN_TIME_COLOR = cocos2d::Color4B::RED;
const cocos2d::Color4B HMMGui::NORMAL_REMAIN_TIME_COLOR = cocos2d::Color4B::WHITE;
const float HMMGui::START_UP_FADE_IN_DURATION = 0.6f;
const float HMMGui::COUNT_BACKWARDS_DURATION = 1.f;
const int HMMGui::COUNT_BACKWARDS_NUMBER = 1;
const float HMMGui::START_UP_DELAY = 2.5f;
    
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
, _timeLabelBg(nullptr)
, _thisHpLabelBg(nullptr)
, _thisHpProgressView(nullptr)
, _thisHpLabel(nullptr)
, _enemyHpProgressView(nullptr)
, _enemyHpLabel(nullptr)
, _enemyHpLabelBg(nullptr)
, _exitButton(nullptr)
, _game(nullptr)
, _commander(nullptr)
, _worldRender(nullptr) {
}
    
HMMGui::~HMMGui() {
    CC_SAFE_RELEASE(_guiView);
    if (_game) {
        _game->getWorld()->removeWorldObserver(this);
        _game = nullptr;
    }
    _commander = nullptr;
    _worldRender = nullptr;
    
    if (_deckRender) {
        _deckRender->markObjectReleased();
    }
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
    _game->getWorld()->addWorldObserver(this);
    
    /**3. init instance */
    _thisCore = nullptr;
    _enemyCore = nullptr;
    if (game->getWorld()->getFaction(_thisFactionIndex)) {
        _thisCore = game->getWorld()->getFaction(_thisFactionIndex)->findFirstUnitByClass(kUnitClass_Core);
        _thisCoreId = _thisCore ? _thisCore->getId() : World::INVALID_ID;
    }
    if (_enemyFactionIndex != INVALID_FACTION_INDEX
        && game->getWorld()->getFaction(_enemyFactionIndex)) {
        _enemyCore = game->getWorld()->getFaction(_enemyFactionIndex)->findFirstUnitByClass(kUnitClass_Core);
        _enemyCoreId = _enemyCore ? _enemyCore->getId() : World::INVALID_ID;
    }
    _gameOver = false;
    
    /**4. init cocos */
    
    //4a. clean up
    const cocos2d::Size& winSize = cocos2d::Director::getInstance()->getWinSize();
    if (!_guiView) {
        _guiView = cocos2d::LayerColor::create();
        _guiView->retain();
    }
    _guiView->removeAllChildren();
    _thisHpLabel = nullptr;
    _thisHpProgressView = nullptr;
    _thisHpLabelBg = nullptr;
    _enemyHpLabel = nullptr;
    _enemyHpProgressView = nullptr;
    _enemyHpLabelBg = nullptr;
    _timeLabel = nullptr;
    _timeLabelBg = nullptr;

    
    //4b. faction info
    initFactionInfo(_game->getWorld()->getFaction(_thisFactionIndex));
    if (_enemyFactionIndex != INVALID_FACTION_INDEX) {
        initFactionInfo(_game->getWorld()->getFaction(_enemyFactionIndex));
    }

    //4c. time label
    {
        cocos2d::Sprite* sprite = cocos2d::Sprite::create("GameImages/battle_ui/ui_clock_bg.png");
        _guiView->addChild(sprite, COVER_UI_ZORDER);
        
        const cocos2d::Size& size = sprite->getContentSize();
        sprite->setPosition(cocos2d::Vec2(winSize.width / 2, winSize.height - size.height / 2 - 25.f));
        
        _timeLabel = CocosUtils::createLabel("player", 24, DEFAULT_NUMBER_FONT);
        //_timeLabel->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_LEFT);
        _timeLabel->setPosition(cocos2d::Vec2(size.width * 0.5, 13.f));
        sprite->addChild(_timeLabel);
        
        cocos2d::Node* hint_icon = cocos2d::Sprite::create("GameImages/battle_ui/ui_clock_hint_icon.png");
        hint_icon->setPosition(cocos2d::Vec2(size.width * 0.5f, size.height));
        sprite->addChild(hint_icon);
        _timeLabelBg = sprite;
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
    
    //4e. exit button
    {
        static const string file("GameImages/battle_ui/ui_exit_button.png");
        MenuItemImage* exitItem = MenuItemImage::create(file, file, [this](Ref*) {
            MessageBoxLayer::getInstance()->show(LocalHelper::getString("hint_exitPve"), MessageBoxType::YesNo, [this](Ref*) {
                if (_renderListener) _renderListener->onExitRequest();
            });

        });
        exitItem->setAnchorPoint(Point::ANCHOR_TOP_RIGHT);
        exitItem->setPosition(Point(winSize.width - 5.0f, winSize.height - 5.0f));
        _exitButton = Menu::create(exitItem, nullptr);
        _exitButton->setPosition(Point::ZERO);
        _guiView->addChild(_exitButton, COVER_UI_ZORDER);
    }
    
    //5. anim
    runStartUpAnim();
}
    
void HMMGui::render(const Game *game) {
    if (!game) return;
    
    //1. update time
    int timeInMicrosec = GameConstants::frame2Microsecond(game->getFrame());
    int timeRemainInSec = GameConstants::microsecond2Sec(m_clampi(_gameDuration - timeInMicrosec, 0, _gameDuration));
    if (_timeLabel) {
        _timeLabel->setString(Utils::getFormattedTime(timeRemainInSec));
        
//        if (timeRemainInSec <= ALERT_REMAIN_TIME_IN_SECOND && _timeLabel->getTextColor() != ALERT_REMAIN_TIME_COLOR) {
//            _timeLabel->setTextColor(NORMAL_REMAIN_TIME_COLOR);
//        } else if (timeRemainInSec > ALERT_REMAIN_TIME_IN_SECOND && _timeLabel->getTextColor() != NORMAL_REMAIN_TIME_COLOR){
//            _timeLabel->setTextColor(NORMAL_REMAIN_TIME_COLOR);
//        }
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
    
void HMMGui::setRenderListener(AbstractRender::RenderListener* renderListener) {
    _renderListener = renderListener;
}
    
void HMMGui::onNotifyWorldEvents(const std::vector<World::EventLog>& events) {
    for (int i = 0; i < events.size(); ++i) {
        if (events[i]._type == World::EventLogType::ObjectEliminate) {
            if (_thisCore && events[i]._id == _thisCoreId) {
                _thisCore = nullptr;
                _thisCoreId = World::INVALID_ID;
            } else if (_enemyCore && events[i]._id == _enemyCoreId) {
                _enemyCore = nullptr;
                _enemyCoreId = World::INVALID_ID;
            }
        }
    }
}
    
void HMMGui::onNotifyWorldReleased() {
    _game = nullptr;
    if (_deckRender) _deckRender->markObjectReleased();
}
    
    
void HMMGui::initFactionInfo(const Faction *faction) {
    static const float background_margin_top = 8.f;
    static const float background_padding = 8.f;
    static const float icon_size = 57.f;
    
    if (!faction || !_guiView) return;
    
    bool left = (_thisFactionIndex == faction->getFactionIndex());
    
    const cocos2d::Size& winSize = cocos2d::Director::getInstance()->getWinSize();
    
    cocos2d::Sprite* nameBg = cocos2d::Sprite::create("GameImages/battle_ui/ui_user_info_bg.png");
    _guiView->addChild(nameBg, COVER_UI_ZORDER);
    
    const cocos2d::Size& nameBgSize = nameBg->getContentSize();
    static const float offsetX(192.0f);
    nameBg->setPosition(cocos2d::Vec2(winSize.width / 2 + offsetX * (left ? -1 : 1), winSize.height - nameBgSize.height / 2 - background_margin_top));
    
    // name
    {
        std::string playerName = "player";
        cocos2d::Label* label = CocosUtils::createLabel(playerName, DEFAULT_FONT_SIZE);
        label->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_LEFT);
        label->setPosition(cocos2d::Vec2(76.f, 56.f));
        nameBg->addChild(label);
    }
    
    // progress
    {
        cocos2d::Sprite* sprite = cocos2d::Sprite::create("GameImages/battle_ui/ui_faction_hp_bg.png");
        nameBg->addChild(sprite);
        
        const cocos2d::Size& size = sprite->getContentSize();
        sprite->setPosition(cocos2d::Vec2(73.f, background_padding) + cocos2d::Vec2(size.width / 2, size.height / 2));
        
        cocos2d::ProgressTimer* pt = createProgressTimer(left ? "GameImages/battle_ui/ui_faction_hp_fg_ally.png" : "GameImages/battle_ui/ui_faction_hp_fg_enemy.png");
        pt->setPosition(cocos2d::Vec2(size.width / 2, size.height / 2));
        pt->setMidpoint(left ? cocos2d::Vec2::ANCHOR_BOTTOM_RIGHT : cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
        sprite->addChild(pt);
        
        cocos2d::Label* ptLabel = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        ptLabel->setPosition(pt->getPosition());
        sprite->addChild(ptLabel);
        
        if (left) {
            _thisHpProgressView = pt;
            _thisHpLabel = ptLabel;
            _thisHpLabelBg = nameBg;
        } else {
            _enemyHpProgressView = pt;
            _enemyHpLabel = ptLabel;
            _enemyHpLabelBg = nameBg;
        }
    }
    
    // icon
    {
        cocos2d::Sprite* sprite = cocos2d::Sprite::create("GameImages/public/icon_touxiang_1.png");
        nameBg->addChild(sprite);
        
        sprite->setPosition(cocos2d::Vec2(icon_size / 2 + background_padding, icon_size / 2 + background_padding));
        Rect rect(0.f,
            0.f,
            M_MIN(sprite->getContentSize().width , icon_size),
            M_MIN(sprite->getContentSize().height , icon_size));
        sprite->setTextureRect(rect);
        sprite->getContentSize();
    }
}
    
void HMMGui::runStartUpAnim() {
    
    static const float upper_ui_move_offest = 100.f;
    static const float count_backwards_scale = 2.f;
    static const float count_backwards_opacity = 50;
    
    cocos2d::Sprite* CountBack3 = cocos2d::Sprite::create("GameImages/battle_ui/battle_start_count_backwards_3.png");
    cocos2d::Sprite* CountBack2 = cocos2d::Sprite::create("GameImages/battle_ui/battle_start_count_backwards_2.png");
    cocos2d::Sprite* CountBack1 = cocos2d::Sprite::create("GameImages/battle_ui/battle_start_count_backwards_1.png");
    
    std::function<void (cocos2d::Node*)> setbackCount = [this](cocos2d::Node* target) {
        if (target) {
            target->setScale(count_backwards_scale);
            target->setOpacity(count_backwards_opacity);
            target->setVisible(false);
            const cocos2d::Size& wSize = cocos2d::Director::getInstance()->getWinSize();
            target->setPosition(cocos2d::Vec2(wSize.width / 2, wSize.height / 2));
            _guiView->addChild(target, COVER_UI_ZORDER);
        }
    };
    setbackCount(CountBack3);
    setbackCount(CountBack2);
    setbackCount(CountBack1);
    
    std::function<void (cocos2d::Node*, int)> startBackCountAnim =
        [this](cocos2d::Node* target, int count) {
            if (target) {
                cocos2d::Action* scaleDown1 = cocos2d::Spawn::create(
                    cocos2d::EaseSineIn::create(cocos2d::FadeTo::create(COUNT_BACKWARDS_DURATION / 4, 255)),
                    cocos2d::EaseSineIn::create(cocos2d::ScaleTo::create(COUNT_BACKWARDS_DURATION / 4, 0.8f)),
                    NULL);
                cocos2d::Action* scaleUp = cocos2d::EaseSineIn::create(cocos2d::ScaleTo::create(COUNT_BACKWARDS_DURATION / 4, 1.2f));
                cocos2d::Action* scaleDown2 = cocos2d::EaseSineIn::create(cocos2d::ScaleTo::create(COUNT_BACKWARDS_DURATION / 4, 1));
                cocos2d::Action* fadeup = cocos2d::EaseSineIn::create(cocos2d::FadeOut::create(COUNT_BACKWARDS_DURATION / 4));
                
                cocos2d::Action* seq = cocos2d::Sequence::create(
                    cocos2d::DelayTime::create(START_UP_DELAY + (COUNT_BACKWARDS_NUMBER - count) * COUNT_BACKWARDS_DURATION),
                    cocos2d::CallFunc::create([target]() { target->setVisible(true);}),
                    scaleDown1,
                    scaleUp,
                    scaleDown2,
                    fadeup,
                    cocos2d::RemoveSelf::create(),
                    NULL);
                target->runAction(seq);
                                                                          
            }
    };
    startBackCountAnim(CountBack3, 3);
    startBackCountAnim(CountBack2, 2);
    startBackCountAnim(CountBack1, 1);
    

    std::function<void (cocos2d::Node*)> uiMoveIn = [](cocos2d::Node* target) {
        if (target) {
            cocos2d::Sequence* sq = cocos2d::Sequence::create(
                cocos2d::DelayTime::create(COUNT_BACKWARDS_DURATION * COUNT_BACKWARDS_NUMBER + START_UP_DELAY),
                    cocos2d::EaseSineIn::create(cocos2d::MoveTo::create(START_UP_FADE_IN_DURATION, target->getPosition())),
                nullptr);
            ;
            target->setPosition(target->getPosition() + cocos2d::Vec2(0.f, upper_ui_move_offest));
            target->runAction(sq);
        }
    };
    
    uiMoveIn(_timeLabelBg);
    uiMoveIn(_thisHpLabelBg);
    uiMoveIn(_enemyHpLabelBg);
    uiMoveIn(_exitButton);
    
    if (_deckRender) {
        _deckRender->runStartUpAnim();
    }
}

    
    
}}