//
//  CardNode.h
//  Underworld_Client
//
//  Created by Andy on 16/2/22.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef CardNode_h
#define CardNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "CocosGlobal.h"

USING_NS_CC;
using namespace ui;

class BattleSmallResourceNode;
class CardNode;
namespace UnderWorld { namespace Core {
    class Card;
}}

class CardNodeObserver
{
public:
    virtual ~CardNodeObserver() {}
    virtual void onCardNodeTouchedBegan(CardNode* node) = 0;
    virtual void onCardNodeTouchedEnded(CardNode* node, bool isValid) = 0;
};

class CardNode: public Node
{
public:
    static CardNode* create();
    virtual ~CardNode();
    void registerObserver(CardNodeObserver *observer);
    void update(const UnderWorld::Core::Card* card, float resource);
    void update(const std::string& name, int rarity, int cost, float resource);
    void setSelected(bool selected);
    void checkResource(float count);
    
    // getters
    const UnderWorld::Core::Card* getCard() const;
    const std::string& getCardName() const;
    
protected:
    CardNode();
    virtual bool init() override;
    std::string getIconFile(const std::string& name, bool enable) const;
    BattleSmallResourceNode* readdResourceNode(Node* currentNode, ResourceType type, int count);
    void updateIcon(bool colorful);
    
private:
    CardNodeObserver *_observer;
    Widget *_cardWidget;
    Sprite *_iconSprite;
    Sprite *_qualitySprite;
    BattleSmallResourceNode *_resourceNode;
    std::map<int, Sprite*> _starSprites;
    
    Sprite *_shiningSprite;
    ProgressTimer* _coldDownProgress;
    const UnderWorld::Core::Card* _card;
    std::string _cardName;
    bool _touchInvalid;
};

#endif /* CardNode_h */
