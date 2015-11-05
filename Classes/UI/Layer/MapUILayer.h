//
//  MapUILayer.h
//  Underworld_Client
//
//  Created by Andy on 15/11/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef MapUILayer_h
#define MapUILayer_h

#include "cocos2d.h"
#include "cocos-ext.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;

namespace UnderWorld { namespace Core {
        class UnitType;
}}

// =====================================================
// Unit	Node
// =====================================================

class MapUIUnitNode;

class MapUIUnitNodeObserver
{
public:
    virtual ~MapUIUnitNodeObserver() {}
    virtual void onMapUIUnitNodeTouchedBegan(MapUIUnitNode* node) = 0;
    virtual void onMapUIUnitNodeTouchedEnded(MapUIUnitNode* node) = 0;
};

class MapUIUnitNode: public Node
{
public:
    static MapUIUnitNode* create(const UnderWorld::Core::UnitType* type);
    virtual ~MapUIUnitNode();
    void registerObserver(MapUIUnitNodeObserver *observer);
    const UnderWorld::Core::UnitType* getUnitType() const;
    
protected:
    MapUIUnitNode();
    bool init(const UnderWorld::Core::UnitType* type);
    
private:
    MapUIUnitNodeObserver *_observer;
    const UnderWorld::Core::UnitType* _unitType;
};

// =====================================================
// Layer
// =====================================================

class MapUILayerObserver
{
public:
    virtual ~MapUILayerObserver() {}
};

class MapUILayer: public LayerColor, public TableViewDelegate, public TableViewDataSource, public MapUIUnitNodeObserver
{
public:
    static MapUILayer* create(const std::string& myAccount, const std::string& opponentsAccount);
    virtual ~MapUILayer();
    void registerObserver(MapUILayerObserver *observer);
    
protected:
    MapUILayer();
    
    // LayerColor
    bool init(const std::string& myAccount, const std::string& opponentsAccount);
    virtual void onEnter() override;
    
    // TableViewDelegate
    virtual void tableCellTouched(TableView* table, TableViewCell* cell) override;
    
    // TableViewDataSource
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override;
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(TableView *table) override;
    
    // MapUIUnitNodeObserver
    virtual void onMapUIUnitNodeTouchedBegan(MapUIUnitNode* node) override;
    virtual void onMapUIUnitNodeTouchedEnded(MapUIUnitNode* node) override;
    
private:
    MapUILayerObserver *_observer;
    TableView *_tableView;
    Size _unitNodeSize;
    Size _cellSize;
    Label *_timeLabel;
    Label *_nextWaveTimeLabel;
    Label *_energyLabel;
    Label *_unitCostLabel;
    ProgressTimer *_myHpProgress;
    Label *_myHpPercentageLabel;
    ProgressTimer *_opponentsHpProgress;
    Label *_opponentsHpPercentageLabel;
    MenuItem *_sendTroopMenuItem;
    MenuItem *_pauseMenuItem;
};

#endif /* MapUILayer_h */
