//
//  BattleDeckTestNode.h
//  Underworld_Client
//
//  Created by Andy on 16/2/22.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef BattleDeckTestNode_h
#define BattleDeckTestNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "CocosGlobal.h"

USING_NS_CC;
using namespace ui;

class BattleSmallResourceNode;
class BattleDeckTestNode;

class BattleDeckTestNodeObserver
{
public:
    virtual ~BattleDeckTestNodeObserver() {}
    virtual void onBattleDeckTestNodeTouchedBegan(BattleDeckTestNode* node) = 0;
    virtual void onBattleDeckTestNodeTouchedEnded(BattleDeckTestNode* node, bool isValid) = 0;
    virtual void onBattleDeckTestNodeTouchedCanceled(BattleDeckTestNode* node) = 0;
};

class BattleDeckTestNode: public Node
{
public:
    static BattleDeckTestNode* create(const std::string& name, const std::string& renderKey, bool isHero, int rarity);
    virtual ~BattleDeckTestNode();
    void registerObserver(BattleDeckTestNodeObserver *observer);
    void update(const std::string& name, const std::string& renderKey, bool isHero, int rarity);
    void setSelected(bool selected);
    
    // getters
    const std::string& getUnitName() const;
    
protected:
    BattleDeckTestNode();
    bool init(const std::string& name, const std::string& renderKey, bool isHero, int rarity);
    std::string getIconFile(const std::string& name, bool enable) const;
    BattleSmallResourceNode* readdResourceNode(Node* currentNode, ResourceType type, int count);
    
private:
    BattleDeckTestNodeObserver *_observer;
    Widget *_cardWidget;
    Button *_addButton;
    Sprite *_iconSprite;
    Sprite *_qualitySprite;
    Node* _countNode;
    Label *_countLabel;
    BattleSmallResourceNode *_goldNode;
    BattleSmallResourceNode *_woodNode;
    std::map<int, Sprite*> _starSprites;
    
    Sprite *_shiningSprite;
    std::string _unitName;
    bool _touchInvalid;
};

#endif /* BattleDeckTestNode_h */
