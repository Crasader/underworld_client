//
//  CardOpNode.h
//  Underworld_Client
//
//  Created by Andy on 16/8/12.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardOpNode_h
#define CardOpNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "BattleDeckUI.h"

USING_NS_CC;

class DeckCard;
class AbstractData;

class CardOpNodeObserver
{
public:
    virtual ~CardOpNodeObserver() {}
    virtual void onCardOpNodeClicked() = 0;
    virtual void onCardOpNodeClickedButton(DeckCardOpType type, const AbstractData* data) = 0;
};

class CardOpNode : public ui::Widget
{
public:
    static CardOpNode* create();
    virtual ~CardOpNode();
    void registerObserver(CardOpNodeObserver* observer);
    void setCard(const AbstractData* data);
    void setTypes(const std::vector<DeckCardOpType>& types);
    const AbstractData* getCardData() const;
    
private:
    CardOpNode();
    virtual bool init() override;
    void resetPositions();
    
private:
    CardOpNodeObserver* _observer;
    DeckCard* _icon;
    ui::Scale9Sprite* _hint;
    static Size _hintMinSize;
    std::vector<DeckCardOpType> _opTypes;
    std::map<DeckCardOpType, UniversalButton*> _buttons;
    bool _touchInvalid;
};

#endif /* CardOpNode_h */
