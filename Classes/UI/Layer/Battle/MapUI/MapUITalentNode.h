//
//  MapUITalentNode.h
//  Underworld_Client
//
//  Created by Andy on 16/2/26.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef MapUITalentNode_h
#define MapUITalentNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "CocosGlobal.h"

USING_NS_CC;
using namespace ui;

namespace UnderWorld { namespace Core {
    class Camp;
}}

class MapUITalentNode;

class MapUITalentNodeObserver
{
public:
    virtual ~MapUITalentNodeObserver() {}
    virtual void onMapUITalentNodeClickedButton(MapUITalentNode* node) = 0;
};

class MapUITalentNode: public Node
{
public:
    static MapUITalentNode* create(const UnderWorld::Core::Camp* camp, int idx);
    virtual ~MapUITalentNode();
    void registerObserver(MapUITalentNodeObserver *observer);
    
    // getters
    const UnderWorld::Core::Camp* getCamp() const;
    int getIdx() const;
    
protected:
    MapUITalentNode();
    bool init(const UnderWorld::Core::Camp* camp, int idx);
    std::string getIconFile(bool enable) const;
    
private:
    MapUITalentNodeObserver *_observer;
    const UnderWorld::Core::Camp* _camp;
    int _idx;
};

#endif /* MapUITalentNode_h */
