//
//  MapUILayer.h
//  Underworld_Client
//
//  Created by Andy on 15/11/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef MapUILayer_h
#define MapUILayer_h

#include "cocos2d.h"
#include "MapUICardDeck.h"

USING_NS_CC;

class MapUILayerObserver
{
public:
    virtual ~MapUILayerObserver() {}
    virtual bool onMapUILayerIsGameOver() const = 0;
    virtual void onMapUILayerClickedPauseButton() = 0;
    
    virtual void onMapUILayerCardSelected(const UnderWorld::Core::Card* card, int idx) = 0;
    
    virtual void onMapUILayerTouchMoved(const UnderWorld::Core::Card* card, const Point& position, bool inDeck) = 0;
    virtual void onMapUILayerTouchEnded(const UnderWorld::Core::Card* card, int idx, const Point& position) = 0;
};

class MapUILayer
: public LayerColor
, public MapUICardDeckObserver
{
public:
    static MapUILayer* create(const std::string& myAccount, const std::string& opponentsAccount);
    virtual ~MapUILayer();
    void registerObserver(MapUILayerObserver *observer);
    void reload();
    void updateMyHpProgress(int progress);
    void updateOpponentsHpProgress(int progress);
    void updateRemainingTime(int time);
    void pauseGame();
    void resumeGame();
    void clearHighlightedCard();
    
    // card deck
    void createCardDeck(int count);
    void insertCard(const UnderWorld::Core::Card* card);
    void removeCard(const UnderWorld::Core::Card* card);
    void updateCardDeckCountDown(float time);
    void updateCardDeckResource(const std::map<std::string, float>& resources);
    
protected:
    MapUILayer();
    
    // LayerColor
    bool init(const std::string& myAccount, const std::string& opponentsAccount);
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchMoved(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // MapUICardDeckObserver
    virtual void onMapUICardDeckUnitTouchedBegan(const UnderWorld::Core::Card* card, int idx) override;
    virtual void onMapUICardDeckUnitTouchedEnded(const UnderWorld::Core::Card* card, int idx) override;
    
    void createUserInfo(bool left, const std::string& account);
    bool isGameOver() const;
    bool isPointInTableView(const Point& point);
    void setHighlightedCard(int idx);
    
private:
    MapUILayerObserver *_observer;
    bool _isTouchingTableView;
    int _highlightedCard;
    std::pair<const UnderWorld::Core::Card*, int> _selectedCard;
    // ======================== UI =============================
    Label *_timeLabel;
    ProgressTimer *_myHpProgress;
    Label *_myHpPercentageLabel;
    ProgressTimer *_opponentsHpProgress;
    Label *_opponentsHpPercentageLabel;
    MenuItem *_pauseMenuItem;
    MapUICardDeck* _cardDeck;
};

#endif /* MapUILayer_h */
