//
//  MapUILayer.h
//  Underworld_Client
//
//  Created by Andy on 15/11/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef MapUILayer_h
#define MapUILayer_h

#include "CardDeck.h"

typedef std::pair<CardDeck*, int> CardInfo;

enum class CardDeckType {
    Unit,
    Spell
};

class MapUILayerObserver
{
public:
    virtual ~MapUILayerObserver() {}
    virtual bool onMapUILayerIsGameOver() const = 0;
    virtual void onMapUILayerClickedPauseButton() = 0;
    
    virtual void onMapUILayerCardSelected(int card, int idx) = 0;
    
    virtual void onMapUILayerTouchMoved(int card, const Point& position) = 0;
    virtual void onMapUILayerTouchEnded(int card, int idx, const Point& position) = 0;
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
    void insertCard(CardDeckType type, int card, bool animated);
    void insertCard(CardDeckType type, const HMMCard* card, bool animated);
    void removeCard(CardDeckType type, const HMMCard* card, int index);
    void clear(CardDeckType type);
    void updateNextCard(const HMMCard* card);
    void updateCardCD(CardDeckType type, int idx, float percentage);
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
    virtual void onCardDeckTouchedBegan(CardDeck* node, int card, int idx) override;
    virtual void onCardDeckTouchedEnded(CardDeck* node, int card, int idx) override;
    
    void createUserInfo(bool left, const std::string& account);
    bool isGameOver() const;
    void reorderDecks();
    CardDeck* getDeck(CardDeckType type) const;
    void setHighlightedCard(CardDeck* deck, int idx);
    void setCardInfo(std::pair<CardDeck*, int>& data, CardDeck* deck, int idx) const;
    void clearCardInfo(std::pair<CardDeck*, int>& data) const;
    void addResourceNode();
    
private:
    MapUILayerObserver *_observer;
    bool _isTouchingTableView;
    float _decksTotalWidth;
    CardInfo _highlightedCardInfo;
    CardInfo _selectedCardInfo;
    int _selectedCard;
    
    // cocos2d
    Label *_timeLabel;
    ProgressTimer *_myHpProgress;
    Label *_myHpPercentageLabel;
    ProgressTimer *_opponentsHpProgress;
    Label *_opponentsHpPercentageLabel;
    MenuItem *_pauseMenuItem;
    Node *_resourceNode;
    std::unordered_map<std::string, Label*> _resourceLabels;
    std::map<CardDeckType, CardDeck*> _decks;
};

#endif /* MapUILayer_h */
