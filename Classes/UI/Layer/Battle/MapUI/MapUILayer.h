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
#include "MapUIUpgradeNode.h"
#include "UnitType.h"

USING_NS_CC;
USING_NS_CC_EXT;

class BattleResourceNode;

class MapUILayerObserver
{
public:
    virtual ~MapUILayerObserver() {}
    virtual bool onMapUILayerIsGameOver() const = 0;
    virtual void onMapUILayerClickedPauseButton() = 0;
    virtual bool onMapUILayerIsHeroAlive(const UnderWorld::Core::Camp* camp) const = 0;
    
    virtual ssize_t onMapUILayerCampsCount(UnderWorld::Core::UnitClass uc) const = 0;
    virtual const UnderWorld::Core::Camp* onMapUILayerCampAtIndex(UnderWorld::Core::UnitClass uc, ssize_t idx) const = 0;
    
    virtual void onMapUILayerUnitSelected(const UnderWorld::Core::Camp* camp) = 0;
    virtual void onMapUILayerUnitTouched(const UnderWorld::Core::Camp* camp) = 0;
    virtual void onMapUILayerUnitAdd(const UnderWorld::Core::Camp* camp) = 0;
    virtual void onMapUILayerUnitUpgrade(const UnderWorld::Core::Camp* camp) = 0;
    
    virtual void onMapUILayerTouchMoved(const UnderWorld::Core::Camp* camp, const Point& position) = 0;
    virtual void onMapUILayerTouchCancelled(const UnderWorld::Core::Camp* camp) = 0;
    virtual void onMapUILayerTouchEnded(const UnderWorld::Core::Camp* camp, const Point& position) = 0;
};

class MapUILayer
: public LayerColor
, public TableViewDataSource
, public MapUIUnitNodeObserver
, public CampInfoNodeObserver
, public MapUIUpgradeNodeObserver
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
    void updateGold(int cnt, float decimalCnt);
    void updateWood(int cnt, float decimalCnt);
    void pauseGame();
    void resumeGame();
    bool isPointInTableView(const Point& point);
    void clearHighlightedCamp();
    void removeUpgradeNode();
    
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
    virtual void onMapUIUnitNodeClickedAddButton(const UnderWorld::Core::Camp* camp) override;
    virtual void onMapUIUnitNodeClickedUpgradeButton(MapUIUnitNode* node) override;
    virtual void onMapUIUnitNodeTouchedBegan(const UnderWorld::Core::Camp* camp) override;
    virtual void onMapUIUnitNodeTouchedEnded(const UnderWorld::Core::Camp* camp, bool isValid) override;
    
    // CampInfoNodeObserver
    virtual void onCampInfoNodeClickedIcon(CampInfoNode* pSender, const UnderWorld::Core::UnitBase* unit) override;
    
    // MapUIUpgradeNodeObserver
    virtual void onMapUIUpgradeNodeClickedButton(const UnderWorld::Core::Camp* camp, int idx) override;
    
    void createUserInfo(bool left, const std::string& account);
    bool isGameOver() const;
    void onUnitTouched(const UnderWorld::Core::Camp* camp);
    void createTableViews();
    Node* createTableView(UnderWorld::Core::UnitClass uc, Node* parent);
    UnderWorld::Core::UnitClass getUnitClass(TableView* table) const;
    TableView* getTableView(const UnderWorld::Core::Camp* camp) const;
    ssize_t getCellsCount(TableView* table) const;
    ssize_t getCellsCount(UnderWorld::Core::UnitClass uc) const;
    Rect getTableViewBoundingBox(UnderWorld::Core::UnitClass uc) const;
    void setHighlightedCamp(TableView* table, const UnderWorld::Core::Camp* camp, bool callback = false, bool ignoreProduction = false, bool check = true);
    void createUpgradeNode(const UnderWorld::Core::Camp* camp);
    
private:
    MapUILayerObserver *_observer;
    Size _tableViewMaxSize;
    Point _tableViewPos;
    Size _cellSize;
    std::pair<TableView*, const UnderWorld::Core::Camp*> _highlightedCamp;
    bool _isTouchingTableView;
    const UnderWorld::Core::Camp* _selectedCamp;
    int _createdTablesIdx;
    // ======================== UI =============================
    std::map<UnderWorld::Core::UnitClass, TableView*> _tableViews;
    Label *_timeLabel;
    BattleResourceNode *_goldResourceNode;
    BattleResourceNode *_woodResourceNode;
    ProgressTimer *_myHpProgress;
    Label *_myHpPercentageLabel;
    ProgressTimer *_opponentsHpProgress;
    Label *_opponentsHpPercentageLabel;
    MenuItem *_pauseMenuItem;
    std::vector<CampInfoNode*> _campInfoNodes;
    MapUIUpgradeNode* _upgradeNode;
};

#endif /* MapUILayer_h */
