//
//  HMMGui.h
//  Underworld_Client
//
//  Created by Mac on 16/7/28.
//
//

#ifndef HMMGui_h
#define HMMGui_h

#include "cocos2d.h"

#include "Gui.h"
#include "GameConstants.h"
#include "CocosGlobal.h"
#include "World.h"

class BattleSmallResourceNode;
class CardConfigData;

namespace UnderWorld{ namespace Core{
    
class Faction;
class Unit;
class HMMDeck;
class HMMCard;
class HMMCardType;
class HMMDeckRender;
    
class HMMGui : public Gui, public WorldObserver {
public:
    // zorders
    static const int COVER_UI_ZORDER;
    static const int DECK_UI_ZORDER;
    
    // consts
    static const int INVALID_FACTION_INDEX;
    static const int ALERT_REMAIN_TIME_IN_SECOND;
    static const cocos2d::Color4B ALERT_REMAIN_TIME_COLOR;
    static const cocos2d::Color4B NORMAL_REMAIN_TIME_COLOR;
    static const float START_UP_FADE_IN_DURATION;
    static const float COUNT_BACKWARDS_DURATION;
    static const int COUNT_BACKWARDS_NUMBER;
    static const float START_UP_DELAY;
    
private:
    /** data */
    int _thisFactionIndex;
    int _enemyFactionIndex;
    microsec_t _gameDuration;
    
    /** instance */
    AbstractRender::RenderListener* _renderListener;
    const Unit* _thisCore;
    creatureid_t _thisCoreId;
    const Unit* _enemyCore;
    creatureid_t _enemyCoreId;
    bool _gameOver;
    
    /** cocos */
    cocos2d::Node* _guiView;
    cocos2d::Label* _timeLabel;
    cocos2d::Node* _timeLabelBg;
    cocos2d::ProgressTimer* _thisHpProgressView;
    cocos2d::Label* _thisHpLabel;
    cocos2d::Node* _thisHpLabelBg;
    cocos2d::ProgressTimer* _enemyHpProgressView;
    cocos2d::Label* _enemyHpLabel;
    cocos2d::Node* _enemyHpLabelBg;
    cocos2d::Menu* _exitButton;
    HMMDeckRender* _deckRender;
    
    /** refs */
    const Game* _game;
    Commander* _commander;
    WorldRender* _worldRender;
    
public:
    HMMGui();
    virtual ~HMMGui();
    
    /** override Gui */
    virtual void init(const Game* game, Commander* commander, WorldRender* worldRender) override;
    virtual void render(const Game* game) override;
    virtual void setRenderListener(AbstractRender::RenderListener* renderListener) override;

    
    /** override WorldObserver */
    virtual void onNotifyWorldEvents(const std::vector<World::EventLog>& events) override;
    virtual void onNotifyWorldReleased() override;
    
    /* getters */
    cocos2d::Node* getGuiView() override        {return _guiView;}
    
    
private:
    void initFactionInfo(const Faction* faction);
    void runStartUpAnim();
    
};
    
}}

#endif /* HMMGui_h */
