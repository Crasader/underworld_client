//
//  BattleDeckUnitNode.h
//  Underworld_Client
//
//  Created by Andy on 16/2/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef BattleDeckUnitNode_h
#define BattleDeckUnitNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "CocosGlobal.h"

USING_NS_CC;
using namespace ui;

namespace UnderWorld { namespace Core {
    class Camp;
}}

class BattleDeckUnitNode;

class BattleDeckUnitNodeObserver
{
public:
    virtual ~BattleDeckUnitNodeObserver() {}
    virtual void onBattleDeckUnitNodeTouchedBegan(const UnderWorld::Core::Camp* camp) = 0;
    virtual void onBattleDeckUnitNodeTouchedEnded(const UnderWorld::Core::Camp* camp, bool isValid) = 0;
    virtual void onBattleDeckUnitNodeTouchedCanceled(const UnderWorld::Core::Camp* camp) = 0;
};

class BattleDeckUnitNode: public Node
{
public:
    static BattleDeckUnitNode* create(const UnderWorld::Core::Camp* camp);
    virtual ~BattleDeckUnitNode();
    void registerObserver(BattleDeckUnitNodeObserver *observer);
    void reuse(const UnderWorld::Core::Camp* camp, ssize_t idx);
    void update(bool reuse);
    void setSelected(bool selected);
    void setTouched(bool touched, bool isGameOver);
    
    // getters
    const UnderWorld::Core::Camp* getCamp() const;
    
protected:
    BattleDeckUnitNode();
    bool init(const UnderWorld::Core::Camp* camp);
    std::string getIconFile(const UnderWorld::Core::Camp* camp, bool enable) const;
    bool isHero(const UnderWorld::Core::Camp* camp) const;
    void addTouchedAction(bool touched, bool animated);
    
private:
    BattleDeckUnitNodeObserver *_observer;
    Widget *_cardWidget;
    Button *_addButton;
    Sprite *_iconSprite;
    Sprite *_qualitySprite;
    Sprite *_goldSprite;
    Sprite *_woodSprite;
    Label *_countLabel;
    Label *_goldLabel;
    Label *_woodLabel;
    std::map<int, Sprite*> _starSprites;
    
    Sprite *_shiningSprite;
    const UnderWorld::Core::Camp* _camp;
    ssize_t _idx;
    bool _touchInvalid;
    
    bool _isIconTouched;
};

#endif /* BattleDeckUnitNode_h */
