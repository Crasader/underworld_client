//
//  BattleDeckLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/8/10.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef BattleDeckLayer_h
#define BattleDeckLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <unordered_map>

USING_NS_CC;

class DeckData;
class TabButton;
class UniversalButton;

enum class DeckTabType {
    None,
    All,
    Heroes,
    Soldiers,
    Spells,
};

class BattleDeckLayerObserver
{
public:
    virtual ~BattleDeckLayerObserver() {}
};

class BattleDeckLayer : public LayerColor
{
public:
    static BattleDeckLayer* create();
    virtual ~BattleDeckLayer();
    void registerObserver(BattleDeckLayerObserver *observer);
    
protected:
    BattleDeckLayer();
    
    // LayerColor
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    void createLeftNode();
    void createRightNode();
    
    void updateCardsCount(int count);
    
    // functions
    void saveThisDeck();
    void loadDeck(int idx);
    void setDefaultDeck();
    void setCardType(DeckTabType type);
    std::string getCardTabName(DeckTabType type) const;
    
private:
    BattleDeckLayerObserver *_observer;
    
    // UI
    Node* _background;
    std::vector<TabButton*> _deckTabButtons;
    std::vector<TabButton*> _cardTabButtons;
    Label* _cardsCountLabel;
    UniversalButton* _sortTypeButton;
    
    // Data
    int _thisDeckIdx;
    DeckData* _thisDeckData;
    std::unordered_map<int, DeckData*> _editedDeckData;
    
    DeckTabType _thisCardType;
};

#endif /* BattleDeckLayer_h */
