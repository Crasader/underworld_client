//
//  HMMDeckRender.h
//  Underworld_Client
//
//  Created by wenchengye on 16/7/30.
//
//

#ifndef HMMDeckRender_h
#define HMMDeckRender_h

#include "cocos2d.h"

#include "GameConstants.h"

namespace UnderWorld{ namespace Core{
    
class Commander;
class Game;
class HMMDeck;
class HMMCardView;
class HMMCardRender;
class WorldRender;
    
class HMMDeckRender : public cocos2d::LayerColor {
public:
    /** zorder */
    static const int BACKGROUND_ZORDER;
    static const int RESOURCE_ZORDER;
    static const int CARD_ZORDER;
    
    /** consts */
    static const std::string BATTLE_RESOURCE_NAME;
    
private:
    /** instance */
    std::vector<HMMCardRender*> _cardRenders;
    
    /** cocos */
    cocos2d::Node* _background;
    std::vector<cocos2d::ProgressTimer*> _resourceViews;
    cocos2d::Node* _cardRegion;
    std::vector<HMMCardView*> _cardViews;
    
    /** refs */
    Commander* _commander;
    WorldRender* _worldRender;
    
public:
    /** init */
    static HMMDeckRender* create(const HMMDeck* deck,
        Commander* commander,
        const Game* game,
        WorldRender* worldRender);
    virtual ~HMMDeckRender();
    virtual bool init(const HMMDeck* deck,
        Commander* commander,
        const Game* game,
        WorldRender* worldRender);
    
    /** upate */
    void render(const HMMDeck* deck, const Game* game);
    void updateBattleResource(microres_t amount);
    
    /** touch event */
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    
    /** getter */
    cocos2d::Node* getDeckView()        {return this;}
    
private:
    HMMDeckRender();
};

    
    
}}

#endif /* HMMDeckRender_h */
