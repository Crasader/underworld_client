//
//  MapUIUnitNode.h
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef MapUIUnitNode_h
#define MapUIUnitNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "CocosGlobal.h"

USING_NS_CC;
using namespace ui;

namespace UnderWorld { namespace Core {
    class Camp;
}}

class MapUIUnitNode;

class MapUIUnitNodeObserver
{
public:
    virtual ~MapUIUnitNodeObserver() {}
    virtual void onMapUIUnitNodeClickedAddButton(MapUIUnitNode* node) = 0;
    virtual void onMapUIUnitNodeClickedUpgradeButton(MapUIUnitNode* node) = 0;
    
    virtual void onMapUIUnitNodeTouchedBegan(MapUIUnitNode* node) = 0;
    virtual void onMapUIUnitNodeTouchedEnded(MapUIUnitNode* node, bool isValid) = 0;
    virtual void onMapUIUnitNodeTouchedCanceled(MapUIUnitNode* node) = 0;
};

class MapUIUnitNode: public Node
{
public:
    static MapUIUnitNode* create(const UnderWorld::Core::Camp* camp);
    virtual ~MapUIUnitNode();
    void registerObserver(MapUIUnitNodeObserver *observer);
    void reuse(const UnderWorld::Core::Camp* camp, ssize_t idx, int gold, int wood);
    void update(bool reuse, int gold, int wood);
    void setSelected(bool selected);
    void setTouched(bool touched, bool isGameOver);
    
    // getters
    const UnderWorld::Core::Camp* getCamp() const;
    ssize_t getIdx() const;
    
protected:
    MapUIUnitNode();
    bool init(const UnderWorld::Core::Camp* camp);
    std::string getIconFile(const UnderWorld::Core::Camp* camp, bool enable) const;
    bool isHero(const UnderWorld::Core::Camp* camp) const;
    void addTouchedAction(bool touched, bool animated);
    
private:
    MapUIUnitNodeObserver *_observer;
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
    Sprite *_maxIconSprite;
    ProgressTimer* _coldDownProgress;
    const UnderWorld::Core::Camp* _camp;
    ssize_t _idx;
    bool _touchInvalid;
    
    bool _isIconTouched;
};

#endif /* MapUIUnitNode_h */
