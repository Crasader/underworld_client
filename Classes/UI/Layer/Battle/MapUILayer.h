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
    class Camp;
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
    virtual void onMapUIUnitNodeUpdated(MapUIUnitNode* node) = 0;
};

class MapUIUnitNode: public Node
{
public:
    static MapUIUnitNode* create(const UnderWorld::Core::Camp* camp, ssize_t idx);
    virtual ~MapUIUnitNode();
    void registerObserver(MapUIUnitNodeObserver *observer);
    void reuse(const UnderWorld::Core::Camp* camp, ssize_t idx);
    void update(bool reuse);
    void setSelected(bool selected);
    
    // getters
    const UnderWorld::Core::Camp* getCamp() const;
    ssize_t getIdx() const;
    
protected:
    MapUIUnitNode();
    bool init(const UnderWorld::Core::Camp* camp, ssize_t idx);
    std::string getIconFile(const UnderWorld::Core::Camp* camp) const;
    
private:
    MapUIUnitNodeObserver *_observer;
    Button *_iconButton;
    ResourceButton *_resourceButton;
    Label *_countLabel;
    const UnderWorld::Core::Camp* _camp;
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
    virtual void onMapUILayerUnitSelected(MapUIUnitNode* node) = 0;
    virtual void onMapUILayerClickedPauseButton() = 0;
    virtual ssize_t onMapUILayerCampsCount() = 0;
    virtual const UnderWorld::Core::Camp* onMapUILayerCampAtIndex(ssize_t idx) = 0;
};

class MapUILayer: public LayerColor, public TableViewDataSource, public MapUIUnitNodeObserver
{
public:
    static MapUILayer* create(const std::string& myAccount, const std::string& opponentsAccount);
    virtual ~MapUILayer();
    void registerObserver(MapUILayerObserver *observer);
    void reload();
    void updateMyHpProgress(int progress);
    void updateOpponentsHpProgress(int progress);
    void updateWaveTime(int time);
    void updateRemainingTime(int time);
    void updatePopulation(int count, int maxCount);
    void updateGold(int count);
    void pauseGame();
    void resumeGame();
    
protected:
    MapUILayer();
    
    // LayerColor
    bool init(const std::string& myAccount, const std::string& opponentsAccount);
    
    // TableViewDataSource
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override;
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(TableView *table) override;
    
    // MapUIUnitNodeObserver
    virtual void onMapUIUnitNodeTouchedEnded(MapUIUnitNode* node) override;
    virtual void onMapUIUnitNodeUpdated(MapUIUnitNode* node) override;
    
    void onUnitTouched(MapUIUnitNode* node);
    
private:
    MapUILayerObserver *_observer;
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
};

#endif /* MapUILayer_h */
