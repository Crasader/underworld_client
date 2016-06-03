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
    class HMMCard;
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
    static CardNode* create(bool canShake);
    virtual ~CardNode();
    void registerObserver(CardNodeObserver *observer);
    virtual void setPosition(const Vec2& position) override;
    virtual void setPosition(float x, float y) override;
    virtual void setOpacity(GLubyte opacity) override;
    
    // spell cards
    void update(const std::string& name);
    void updateCD(float percentage);
    
    // unit cards
    void update(const UnderWorld::Core::HMMCard* card, float resource);
    void update(const std::string& name, int rarity, int cost, float resource);
    void setSelected(bool selected);
    void checkResource(float count);
    
    // getters
    const UnderWorld::Core::HMMCard* getCard() const;
    const std::string& getCardName() const;
    
protected:
    CardNode();
    bool init(bool canShake);
    std::string getIconFile(const std::string& name, bool enable) const;
    BattleSmallResourceNode* readdResourceNode(Node* currentNode, ::ResourceType type, int count);
    void updateIcon(bool colorful);
    void resetPosition();
    void shake();
    void stopShake();
    
private:
    CardNodeObserver *_observer;
    Widget *_cardWidget;
    Sprite *_iconSprite;
    Sprite *_qualitySprite;
    BattleSmallResourceNode *_resourceNode;
    std::unordered_map<int, Sprite*> _starSprites;
    
    Sprite *_shiningSprite;
    ProgressTimer* _coldDownProgress;
    const UnderWorld::Core::HMMCard* _card;
    std::string _cardName;
    bool _touchInvalid;
    bool _selected;
    bool _canShake;
    bool _isShaking;
    Point _basePoint;
};

#endif /* CardNode_h */
