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

class ResourceButton;

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
    static MapUIUnitNode* create(const UnderWorld::Core::UnitType* type, ssize_t idx);
    virtual ~MapUIUnitNode();
    void registerObserver(MapUIUnitNodeObserver *observer);
    const UnderWorld::Core::UnitType* getUnitType() const;
    ssize_t getIdx() const;
    void addSelectedFlag();
    void removeSelectedFlag();
    
protected:
    MapUIUnitNode();
    bool init(const UnderWorld::Core::UnitType* type, ssize_t idx);
    
private:
    MapUIUnitNodeObserver *_observer;
    Button *_iconButton;
    const UnderWorld::Core::UnitType* _unitType;
    ssize_t _idx;
    bool _touchInvalid;
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
    virtual void onExit() override;
    
    // TableViewDelegate
    virtual void tableCellTouched(TableView* table, TableViewCell* cell) override;
    
    // TableViewDataSource
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override;
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(TableView *table) override;
    
    // MapUIUnitNodeObserver
    virtual void onMapUIUnitNodeTouchedBegan(MapUIUnitNode* node) override;
    virtual void onMapUIUnitNodeTouchedEnded(MapUIUnitNode* node) override;
    
    // test
    void fakeTick(float dt);
    void updateWaveTime(int time);
    void updateRemainingTime(int time);
    
private:
    MapUILayerObserver *_observer;
    TableView *_tableView;
    Size _unitNodeSize;
    Size _cellSize;
    Label *_timeLabel;
    Label *_nextWaveTimeLabel;
    ResourceButton *_energyResourceButton;
    Label *_unitCostLabel;
    ProgressTimer *_myHpProgress;
    Label *_myHpPercentageLabel;
    ProgressTimer *_opponentsHpProgress;
    Label *_opponentsHpPercentageLabel;
    MenuItem *_sendTroopMenuItem;
    MenuItem *_pauseMenuItem;
    int _waveTime;
    int _remainingTime;
    int _selectedUnitIdx;
};

#endif /* MapUILayer_h */
