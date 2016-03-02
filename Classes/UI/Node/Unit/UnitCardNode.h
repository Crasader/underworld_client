//
//  UnitCardNode.h
//  Underworld_Client
//
//  Created by Andy on 16/2/22.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef UnitCardNode_h
#define UnitCardNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "CocosGlobal.h"

USING_NS_CC;
using namespace ui;

class BattleSmallResourceNode;
class UnitCardNode;

class UnitCardNodeObserver
{
public:
    virtual ~UnitCardNodeObserver() {}
    virtual void onUnitCardNodeTouchedBegan(UnitCardNode* node) = 0;
    virtual void onUnitCardNodeTouchedEnded(UnitCardNode* node, bool isValid) = 0;
};

class UnitCardNode: public Node
{
public:
    static UnitCardNode* create(const std::string& name, const std::string& renderKey, int rarity);
    virtual ~UnitCardNode();
    void registerObserver(UnitCardNodeObserver *observer);
    void update(const std::string& name, const std::string& renderKey, int rarity);
    void setSelected(bool selected);
    
    // getters
    const std::string& getUnitName() const;
    
protected:
    UnitCardNode();
    bool init(const std::string& name, const std::string& renderKey, int rarity);
    std::string getIconFile(const std::string& name, bool enable) const;
    BattleSmallResourceNode* readdResourceNode(Node* currentNode, ResourceType type, int count);
    
private:
    UnitCardNodeObserver *_observer;
    Widget *_cardWidget;
    Button *_addButton;
    Sprite *_iconSprite;
    Sprite *_qualitySprite;
    BattleSmallResourceNode *_goldNode;
    std::map<int, Sprite*> _starSprites;
    
    Sprite *_shiningSprite;
    std::string _unitName;
    bool _touchInvalid;
};

#endif /* UnitCardNode_h */
