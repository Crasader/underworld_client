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

// =====================================================
// Unit	Node
// =====================================================

class MapUIUnitNode;

class MapUIUnitNodeObserver
{
public:
    virtual ~MapUIUnitNodeObserver() {}
    virtual void onMapUIUnitNodeSelected(MapUIUnitNode* node) = 0;
};

class MapUIUnitNode: public Node
{
public:
    static MapUIUnitNode* create();
    virtual ~MapUIUnitNode();
    void registerObserver(MapUIUnitNodeObserver *observer);
    
protected:
    MapUIUnitNode();
    
    // LayerColor
    bool init() override;
    
private:
    MapUIUnitNodeObserver *_observer;
};

// =====================================================
// Layer
// =====================================================

class MapUILayerObserver
{
public:
    virtual ~MapUILayerObserver() {}
};

class MapUILayer: public LayerColor, public TableViewDelegate, public TableViewDataSource
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
    
    // TableViewDataSource
    virtual void tableCellTouched(TableView* table, TableViewCell* cell) override;
    
    // TableViewDelegate
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override;
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(TableView *table) override;
    
private:
    MapUILayerObserver *_observer;
    TableView *_tableView;
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
