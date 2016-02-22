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
#include "CampInfoNode.h"
#include "UnitType.h"

USING_NS_CC;
USING_NS_CC_EXT;

class BattleResourceNode;

class MapUILayerObserver
{
public:
    virtual ~MapUILayerObserver() {}
    virtual bool onMapUILayerIsGameOver() = 0;
    virtual void onMapUILayerClickedPauseButton() = 0;
    virtual ssize_t onMapUILayerCampsCount(UnderWorld::Core::UnitClass uc) = 0;
    virtual const UnderWorld::Core::Camp* onMapUILayerCampAtIndex(UnderWorld::Core::UnitClass uc, ssize_t idx) = 0;
    
    virtual void onMapUILayerUnitSelected(const UnderWorld::Core::Camp* camp) = 0;
    virtual void onMapUILayerUnitAdd(const UnderWorld::Core::Camp* camp) = 0;
    virtual void onMapUILayerUnitUpgrade(const UnderWorld::Core::Camp* camp) = 0;
    
    virtual void onMapUILayerTouchMoved(const UnderWorld::Core::Camp* camp, const Point& position) = 0;
    virtual void onMapUILayerTouchCancelled(const UnderWorld::Core::Camp* camp) = 0;
    virtual void onMapUILayerTouchEnded(const UnderWorld::Core::Camp* camp, const Point& position) = 0;
};

class MapUILayer: public LayerColor, public TableViewDataSource, public MapUIUnitNodeObserver, public CampInfoNodeObserver
{
public:
    static MapUILayer* create(const std::string& myAccount, const std::string& opponentsAccount);
    virtual ~MapUILayer();
    void registerObserver(MapUILayerObserver *observer);
    void reload();
    void reloadTable(UnderWorld::Core::UnitClass uc);
#if ENABLE_CAMP_INFO
    void insertCampInfo(size_t idx, const std::vector<std::pair<const UnderWorld::Core::Camp*, const UnderWorld::Core::UnitBase*>>& units);
    void updateCampInfos(size_t idx);
#else
    void insertUnitInfos(size_t idx, const std::vector<const UnderWorld::Core::UnitBase*>& units);
    void updateUnitInfos(size_t idx);
#endif
    void closeAllUnitInfoNodes();
    void updateMyHpProgress(int progress);
    void updateOpponentsHpProgress(int progress);
    void updateRemainingTime(int time);
#if false
    void updateWaveTime(int time, int totalTime);
    void updatePopulation(int count, int maxCount);
#endif
    void updateGoldAndWood(int gold, int wood);
    void pauseGame();
    void resumeGame();
    bool isPointInTableView(const Point& point);
    
protected:
    MapUILayer();
    
    // LayerColor
    bool init(const std::string& myAccount, const std::string& opponentsAccount);
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchMoved(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // TableViewDataSource
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override;
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(TableView *table) override;
    
    // MapUIUnitNodeObserver
    virtual void onMapUIUnitNodeClickedAddButton(MapUIUnitNode* node) override;
    virtual void onMapUIUnitNodeClickedUpgradeButton(MapUIUnitNode* node) override;
    virtual void onMapUIUnitNodeTouchedBegan(MapUIUnitNode* node) override;
    virtual void onMapUIUnitNodeTouchedEnded(MapUIUnitNode* node, bool isValid) override;
    virtual void onMapUIUnitNodeTouchedCanceled(MapUIUnitNode* node) override;
    
    // CampInfoNodeObserver
    virtual void onCampInfoNodeClickedIcon(CampInfoNode* pSender, const UnderWorld::Core::UnitBase* unit) override;
    
    void createUserInfo(bool left, const std::string& account);
    bool isGameOver() const;
    void onUnitTouched(MapUIUnitNode* node);
    void clearTouchedCampIdx();
    void createTableViews();
    Node* createTableView(UnderWorld::Core::UnitClass uc, Node* parent);
    UnderWorld::Core::UnitClass getUnitClass(TableView* table) const;
    ssize_t getCellsCount(TableView* table) const;
    ssize_t getCellsCount(UnderWorld::Core::UnitClass uc) const;
    Rect getTableViewBoundingBox(UnderWorld::Core::UnitClass uc) const;
    
private:
    MapUILayerObserver *_observer;
    Size _tableViewMaxSize;
    Point _tableViewPos;
    Size _cellSize;
    std::pair<TableView*, ssize_t> _selectedCampIdx;
    std::pair<TableView*, ssize_t> _touchedCampIdx;
    bool _isTouchingTableView;
    const UnderWorld::Core::Camp* _selectedCamp;
    // ======================== UI =============================
    std::map<UnderWorld::Core::UnitClass, TableView*> _tableViews;
    Label *_timeLabel;
    BattleResourceNode *_goldResourceNode;
    BattleResourceNode *_woodResourceNode;
#if false
    Label *_nextWaveTimeLabel;
    ProgressTimer *_nextWaveProgress;
    Label *_populationLabel;
#endif
    ProgressTimer *_myHpProgress;
    Label *_myHpPercentageLabel;
    ProgressTimer *_opponentsHpProgress;
    Label *_opponentsHpPercentageLabel;
    MenuItem *_pauseMenuItem;
    std::vector<CampInfoNode*> _campInfoNodes;
};

#endif /* MapUILayer_h */
