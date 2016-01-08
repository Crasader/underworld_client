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
#include "MapUIUnitNode.h"
#include "DisplayIconNode.h"

USING_NS_CC;
USING_NS_CC_EXT;

namespace UnderWorld { namespace Core {
    class Camp;
}}

class ResourceButton;
class UnitInfoNode;

class MapUILayerObserver
{
public:
    virtual ~MapUILayerObserver() {}
    virtual void onMapUILayerUnitSelected(MapUIUnitNode* node) = 0;
    virtual void onMapUILayerClickedPauseButton() = 0;
    virtual ssize_t onMapUILayerCampsCount() = 0;
    virtual const UnderWorld::Core::Camp* onMapUILayerCampAtIndex(ssize_t idx) = 0;
};

class MapUILayer: public LayerColor, public TableViewDataSource, public MapUIUnitNodeObserver, public DisplayIconNodeObserver
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
    
    // DisplayIconNodeObserver
    virtual void onDisplayIconNodeTouchedEnded(int unitId) override;
    
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
    MenuItem *_pauseMenuItem;
    DisplayIconNode *_displayIconNode;
    UnitInfoNode *_unitInfoNode;
    // ======================== test =============================
};

#endif /* MapUILayer_h */
