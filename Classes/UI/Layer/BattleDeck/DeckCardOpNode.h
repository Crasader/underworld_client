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
    virtual void onDeckCardOpNodeClicked() = 0;
    virtual void onDeckCardOpNodeClickedButton(DeckCardOpType type, int cardId) = 0;
};

class DeckCardOpNode : public ui::Widget
{
public:
    static DeckCardOpNode* create();
    virtual ~DeckCardOpNode();
    void registerObserver(DeckCardOpNodeObserver* observer);
    void setCard(int cardId);
    void setTypes(const std::vector<DeckCardOpType>& types);
    int getCardId() const;
    
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
    bool _touchInvalid;
};

#endif /* DeckCardOpNode_h */
