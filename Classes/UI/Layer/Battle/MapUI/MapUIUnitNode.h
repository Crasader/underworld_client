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

class ResourceButton;
class MapUIUnitNode;

class MapUIUnitNodeObserver
{
public:
    virtual ~MapUIUnitNodeObserver() {}
    virtual void onMapUIUnitNodeTouchedBegan(MapUIUnitNode* node) = 0;
    virtual void onMapUIUnitNodeTouchedEnded(MapUIUnitNode* node) = 0;
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
    void setTouched(bool touched);
    
    // getters
    const UnderWorld::Core::Camp* getCamp() const;
    ssize_t getIdx() const;
    
protected:
    MapUIUnitNode();
    bool init(const UnderWorld::Core::Camp* camp);
    std::string getIconFile(const UnderWorld::Core::Camp* camp, bool enable) const;
    bool isHero(const UnderWorld::Core::Camp* camp) const;
    void createResourceButton(ResourceType type, Node* parent);
    void addTouchedAction(bool touched, bool animated);
    
private:
    MapUIUnitNodeObserver *_observer;
    Sprite *_shadow;
    Button *_iconButton;
    Point _iconBasePosition;
    Sprite *_mask;
    Sprite *_resourcesMask;
    ProgressTimer* _spellColdDown;
    std::map<ResourceType, ResourceButton*> _resourceButtons;
    Label *_countLabel;
    const UnderWorld::Core::Camp* _camp;
    ssize_t _idx;
    bool _touchInvalid;
};

#endif /* MapUIUnitNode_h */
