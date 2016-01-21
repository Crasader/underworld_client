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

class ResourceButton;
class UnitInfoNode;

class MapUILayerObserver
{
public:
    virtual ~MapUILayerObserver() {}
    virtual void onMapUILayerUnitSelected(MapUIUnitNode* node) = 0;
    virtual void onMapUILayerClickedPauseButton() = 0;
    virtual ssize_t onMapUILayerCampsCount(UnderWorld::Core::UnitClass uc) = 0;
    virtual const UnderWorld::Core::Camp* onMapUILayerCampAtIndex(UnderWorld::Core::UnitClass uc, ssize_t idx) = 0;
    virtual void onMapUILayerSpellRingMoved(const UnderWorld::Core::Camp* camp, const Point& position) = 0;
    virtual void onMapUILayerSpellRingCancelled() = 0;
    virtual void onMapUILayerTryToCastSpell(const UnderWorld::Core::Camp* camp, const Point& position) = 0;
};

class MapUILayer: public LayerColor, public TableViewDataSource, public MapUIUnitNodeObserver, public CampInfoNodeObserver
{
public:
    static MapUILayer* create(const std::string& myAccount, const std::string& opponentsAccount);
    virtual ~MapUILayer();
    void registerObserver(MapUILayerObserver *observer);
    void reload();
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
    void updateWaveTime(int time);
    void updateRemainingTime(int time);
    void updatePopulation(int count, int maxCount);
    void updateGoldAndWood(int gold, int wood);
    void pauseGame();
    void resumeGame();
    
protected:
    MapUILayer();
    
    // LayerColor
    bool init(const std::string& myAccount, const std::string& opponentsAccount);
    virtual void onEnterTransitionDidFinish() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchMoved(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // TableViewDataSource
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override;
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(TableView *table) override;
    
    // MapUIUnitNodeObserver
    virtual void onMapUIUnitNodeTouchedBegan(MapUIUnitNode* node) override;
    virtual void onMapUIUnitNodeTouchedEnded(MapUIUnitNode* node) override;
    virtual void onMapUIUnitNodeUpdated(MapUIUnitNode* node) override;
    
    // CampInfoNodeObserver
    virtual void onCampInfoNodeClickedIcon(CampInfoNode* pSender, const UnderWorld::Core::UnitBase* unit) override;
    
    void onUnitTouched(MapUIUnitNode* node);
    void createTableViews();
    Node* createTableView(UnderWorld::Core::UnitClass, Node* parent);
    UnderWorld::Core::UnitClass getUnitClass(TableView* table) const;
    ssize_t getCellsCount(TableView* table) const;
    Rect getTableViewBoundingBox(UnderWorld::Core::UnitClass uc) const;
    
private:
    MapUILayerObserver *_observer;
    int _createdTableViewsCount;
    Size _tableViewMaxSize;
    Point _tableViewPos;
    Size _cellSize;
    std::pair<TableView*, ssize_t> _touchedCamp;
    bool _isTouchingHeroTableView;
    const UnderWorld::Core::Camp* _selectedCamp;
    int _goldCount;
    int _woodCount;
    // ======================== UI =============================
    std::map<UnderWorld::Core::UnitClass, TableView*> _tableViews;
    Label *_timeLabel;
    Label *_nextWaveTimeLabel;
    ResourceButton *_goldResourceButton;
    ResourceButton *_woodResourceButton;
    LabelAtlas *_populationLabel;
    ProgressTimer *_myHpProgress;
    Label *_myHpPercentageLabel;
    ProgressTimer *_opponentsHpProgress;
    Label *_opponentsHpPercentageLabel;
    MenuItem *_pauseMenuItem;
    std::vector<CampInfoNode*> _campInfoNodes;
};

#endif /* MapUILayer_h */
