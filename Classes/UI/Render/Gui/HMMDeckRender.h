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
    static const int BATTLE_RESOURCE_AMOUNT_PER_VIEW = 1;
    static const int CARD_INDEX_INVALID = -1;
    static const float TOUCH_MOVE_THRESHOLD;
    
private:
    /** instance */
    std::vector<HMMCardRender*> _cardRenders;
    
    /** cocos */
    cocos2d::Node* _background;
    cocos2d::Node* _resourceBackground;
    cocos2d::ProgressTimer* _resourceProgress1;
    cocos2d::ProgressTimer* _resourceProgress2;
    cocos2d::Label* _resourceLabel;
    cocos2d::Node* _cardRegion;
    std::vector<HMMCardView*> _cardViews;
    
    /** touch event */
    cocos2d::Vec2 _touchBeginPos;
    int _pressingCardIndex;
    cocos2d::Vec2 _pressingCardOriginPos;
    bool _draggingCard;
    bool _placingCard;
    
    /** status */
    int _selectedCardIndex;
    
    /** refs */
    Commander* _commander;
    WorldRender* _worldRender;
    const HMMDeck* _deck;
    const Game* _game;
    
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
    void updateBattleResource(microres_t amount, microres_t max);
    
    /** touch event */
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    
    /** getter */
    cocos2d::Node* getDeckView()        {return this;}
    
private:
    HMMDeckRender();
    
    /** touch event */
    void resetTouchEventStatus();
    int calculatePositionOnWhichCard(const cocos2d::Vec2& pos);
    bool calculatePositionInCardRegion(const cocos2d::Vec2& pos);
    
    /** misc */
    void selectCard(int index);
    void try2UseCard(int cardIndex, const cocos2d::Vec2& pos);
    

};

    
    
}}

#endif /* HMMDeckRender_h */
