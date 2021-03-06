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

// UnderWorld::Core
namespace UnderWorld { namespace Core {
    class HMMCard;
    class HMMCardType;
}}

// Global
class BattleSmallResourceNode;

USING_NS_CC;
using namespace ui;
using namespace UnderWorld::Core;

// Observer
class CardNode;
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
    static CardNode* create(bool canShake);
    virtual ~CardNode();
    
    void registerObserver(CardNodeObserver *observer);
    
    // inherited from "Node"
    virtual void setPosition(const Vec2& position) override;
    virtual void setPosition(float x, float y) override;
    virtual void setOpacity(GLubyte opacity) override;
    
    // spell cards
    void updateWithoutInfo(int idx);
    void updateCD(float percentage);
    
    // unit cards
    void update(const HMMCard* card, float resource);
    void update(int idx, float resource);
    void setSelected(bool selected);
    void checkResource(float count);
    
    // getters
    const HMMCard* getCard() const;
    int getCardId() const;
    int getCost() const;
    
protected:
    CardNode();
    
    bool init(bool canShake);
    void update(int idx, const HMMCardType* ct, float resource);
    std::string getIconFile(int idx, bool enable) const;
    BattleSmallResourceNode* readdResourceNode(Node* currentNode, ::ResourceType type, int count);
    void updateIcon(bool colorful);
    void resetPosition();
    void shake();
    void stopShake();
    
private:
    CardNodeObserver *_observer;
    
    // cocos2d
    Widget *_cardWidget;
    Sprite *_iconSprite;
    Sprite *_qualitySprite;
    BattleSmallResourceNode *_resourceNode;
    std::unordered_map<int, Sprite*> _starSprites;
    Sprite *_shiningSprite;
    ProgressTimer* _coldDownProgress;
    
    const HMMCard* _card;
    int _cardId;
    bool _touchInvalid;
    bool _selected;
    bool _canShake;
    bool _isShaking;
    Point _basePoint;
};

#endif /* CardNode_h */
