//
//  DeckCardOpNode.h
//  Underworld_Client
//
//  Created by Andy on 16/8/12.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef DeckCardOpNode_h
#define DeckCardOpNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class DeckCard;
class UniversalButton;

enum class DeckCardOpType {
    Upgrade,
    Use,
    Info,
    Move
};

class DeckCardOpNodeObserver
{
public:
    virtual ~DeckCardOpNodeObserver() {}
    virtual void onDeckCardOpNodeClickedButton(DeckCardOpType type, DeckCard* card) = 0;
};

class DeckCardOpNode : public Node
{
public:
    static DeckCardOpNode* create();
    virtual ~DeckCardOpNode();
    void registerObserver(DeckCardOpNodeObserver* observer);
    void setCard(DeckCard* card);
    void setTypes(const std::vector<DeckCardOpType>& types);
    
private:
    DeckCardOpNode();
    virtual bool init() override;
    UniversalButton* createButton(DeckCardOpType opType) const;
    void resetPositions();
    
private:
    DeckCardOpNodeObserver* _observer;
    DeckCard* _icon;
    ui::Scale9Sprite* _hint;
    static Size _hintMinSize;
    std::vector<DeckCardOpType> _opTypes;
    std::map<DeckCardOpType, UniversalButton*> _buttons;
    DeckCard* _card;
};

#endif /* DeckCardOpNode_h */
