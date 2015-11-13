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
    class Game;
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
    virtual void onMapUIUnitNodeTouchedEnded(MapUIUnitNode* node) = 0;
};

class MapUIUnitNode: public Node
{
public:
    static MapUIUnitNode* create(const UnderWorld::Core::UnitType* type, ssize_t idx);
    virtual ~MapUIUnitNode();
    void registerObserver(MapUIUnitNodeObserver *observer);
    void update(const UnderWorld::Core::UnitType* type, ssize_t idx);
    void setSelected(bool selected);
    
    // getters
    const UnderWorld::Core::UnitType* getUnitType() const;
    ssize_t getIdx() const;
    
protected:
    MapUIUnitNode();
    bool init(const UnderWorld::Core::UnitType* type, ssize_t idx);
    
private:
    MapUIUnitNodeObserver *_observer;
    Button *_iconButton;
    ResourceButton *_resourceButton;
    Label *_countLabel;
    const UnderWorld::Core::UnitType* _unitType;
    ssize_t _idx;
    bool _touchInvalid;
};

// =====================================================
// TableViewCell
// =====================================================

class MapUIUnitCell: public TableViewCell
{
public:
    CREATE_FUNC(MapUIUnitCell);
    MapUIUnitNode* getUnitNode() const;
    void setUnitNode(MapUIUnitNode* node);
    void resetUnitNode();
    
private:
    MapUIUnitNode* _unitNode;
};

// =====================================================
// Layer
// =====================================================

class MapUILayerObserver
{
public:
    virtual ~MapUILayerObserver() {}
    virtual void onMapUILayerUnitSelected(ssize_t idx) = 0;
    virtual void onMapUILayerClickedPauseButton(bool pause) = 0;
};

class MapUILayer: public LayerColor, public TableViewDelegate, public TableViewDataSource, public MapUIUnitNodeObserver
{
public:
    static MapUILayer* create(const std::string& myAccount, const std::string& opponentsAccount);
    virtual ~MapUILayer();
    void registerObserver(MapUILayerObserver *observer);
    void initWithGame(const UnderWorld::Core::Game* game);
    void updateMyHpProgress(int progress);
    void updateOpponentsHpProgress(int progress);
    void updateWaveTime(int time);
    void updateRemainingTime(int time);
    
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
    virtual void onMapUIUnitNodeTouchedEnded(MapUIUnitNode* node) override;
    
    void onUnitTouched(ssize_t idx);
    void reloadTableView(ssize_t cellsCount);
    
    // ======================== test =============================
    void fakeTick(float dt);
    
private:
    MapUILayerObserver *_observer;
    bool _paused;
    Size _tableViewMaxSize;
    Size _cellSize;
    ssize_t _cellsCount;
    ssize_t _selectedUnitIdx;
    // ======================== UI =============================
    TableView *_tableView;
    Label *_timeLabel;
    Label *_nextWaveTimeLabel;
    ResourceButton *_energyResourceButton;
    Label *_populationLabel;
    Label *_unitCostLabel;
    ProgressTimer *_myHpProgress;
    Label *_myHpPercentageLabel;
    ProgressTimer *_opponentsHpProgress;
    Label *_opponentsHpPercentageLabel;
    MenuItem *_sendTroopMenuItem;
    MenuItem *_pauseMenuItem;
    // ======================== test =============================
    int _waveTime;
    int _remainingTime;
};

#endif /* MapUILayer_h */
