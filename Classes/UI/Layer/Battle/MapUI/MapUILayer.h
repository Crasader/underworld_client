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
    virtual bool onMapUILayerIsHeroAlive(const UnderWorld::Core::Camp* camp) const = 0;
    
    virtual void onMapUILayerUnitSelected(const UnderWorld::Core::Camp* camp) = 0;
    virtual void onMapUILayerUnitTouched(const UnderWorld::Core::Camp* camp) = 0;
    
    virtual void onMapUILayerTouchMoved(const UnderWorld::Core::Camp* camp, const Point& position) = 0;
    virtual void onMapUILayerTouchCancelled(const UnderWorld::Core::Camp* camp) = 0;
    virtual void onMapUILayerTouchEnded(const UnderWorld::Core::Camp* camp, const Point& position) = 0;
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
    bool isPointInTableView(const Point& point);
    void clearHighlightedCamp();
    
    // card deck
    void createCardDeck(const std::vector<const UnderWorld::Core::Camp*>& camps);
    void initCardDeck(const std::set<const UnderWorld::Core::Camp*>& camps);
    void insertCamp(const UnderWorld::Core::Camp* camp);
    void removeCamp(const UnderWorld::Core::Camp* camp);
    void updateCardDeckCountDown(float time);
    void updateCardDeckResource(float count);
    
protected:
    MapUILayer();
    
    // LayerColor
    bool init(const std::string& myAccount, const std::string& opponentsAccount);
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchMoved(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // MapUICardDeckObserver
    virtual void onMapUICardDeckUnitTouchedBegan(const UnderWorld::Core::Camp* camp) override;
    virtual void onMapUICardDeckUnitTouchedEnded(const UnderWorld::Core::Camp* camp) override;
    
    void createUserInfo(bool left, const std::string& account);
    bool isGameOver() const;
    void onUnitTouched(const UnderWorld::Core::Camp* camp);
    void setHighlightedCamp(const UnderWorld::Core::Camp* camp, bool callback = false, bool ignoreProduction = false, bool check = true);
    
private:
    MapUILayerObserver *_observer;
    bool _isTouchingTableView;
    const UnderWorld::Core::Camp* _highlightedCamp;
    const UnderWorld::Core::Camp* _selectedCamp;
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
