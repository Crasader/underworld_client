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
#include "CardDeck.h"

USING_NS_CC;

typedef std::pair<CardDeck*, int> CardInfo;

enum class CardDeckType {
    Unit,
    Skill
};

class MapUILayerObserver
{
public:
    virtual ~MapUILayerObserver() {}
    virtual bool onMapUILayerIsGameOver() const = 0;
    virtual void onMapUILayerClickedPauseButton() = 0;
    
    virtual void onMapUILayerCardSelected(const std::string& card, int idx) = 0;
    
    virtual void onMapUILayerTouchMoved(const std::string& card, const Point& position) = 0;
    virtual void onMapUILayerTouchEnded(const std::string& card, int idx, const Point& position) = 0;
};

class MapUILayer
: public LayerColor
, public CardDeckObserver
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
    bool isPointInDeck(const Point& point) const;
    
    // card deck
    void createCardDeck(CardDeckType type, int count);
    void insertCard(CardDeckType type, const std::string& name);
    void insertCard(CardDeckType type, const UnderWorld::Core::Card* card);
    void removeCard(CardDeckType type, const UnderWorld::Core::Card* card, int index);
    void updateNextCard(const UnderWorld::Core::Card* card);
    void updateCountDown(float time, float duration);
    void updateResource(const std::unordered_map<std::string, float>& resources);
    
protected:
    MapUILayer();
    
    // LayerColor
    bool init(const std::string& myAccount, const std::string& opponentsAccount);
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchMoved(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // CardDeckObserver
    virtual void onCardDeckTouchedBegan(CardDeck* node, const std::string& card, int idx) override;
    virtual void onCardDeckTouchedEnded(CardDeck* node, const std::string& card, int idx) override;
    
    void createUserInfo(bool left, const std::string& account);
    bool isGameOver() const;
    void reorderDecks();
    CardDeck* getDeck(CardDeckType type) const;
    void setHighlightedCard(CardDeck* deck, int idx);
    void setCardInfo(std::pair<CardDeck*, int>& data, CardDeck* deck, int idx) const;
    void clearCardInfo(std::pair<CardDeck*, int>& data) const;
    
private:
    MapUILayerObserver *_observer;
    bool _isTouchingTableView;
    float _decksTotalWidth;
    CardInfo _highlightedCardInfo;
    CardInfo _selectedCardInfo;
    std::string _selectedCard;
    // ======================== UI =============================
    Label *_timeLabel;
    ProgressTimer *_myHpProgress;
    Label *_myHpPercentageLabel;
    ProgressTimer *_opponentsHpProgress;
    Label *_opponentsHpPercentageLabel;
    MenuItem *_pauseMenuItem;
    std::map<CardDeckType, CardDeck*> _decks;
};

#endif /* MapUILayer_h */
