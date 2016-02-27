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
#include "MapUITable.h"
#include "CampInfoNode.h"
#include "MapUIUpgradeNode.h"
#include "UnitType.h"

USING_NS_CC;

class BattleResourceNode;

class MapUILayerObserver
{
public:
    virtual ~MapUILayerObserver() {}
    virtual bool onMapUILayerIsGameOver() const = 0;
    virtual void onMapUILayerClickedPauseButton() = 0;
    virtual bool onMapUILayerIsHeroAlive(const UnderWorld::Core::Camp* camp) const = 0;
    
    virtual const std::vector<const UnderWorld::Core::Camp*>& onMapUILayerGetCamps(UnderWorld::Core::UnitClass uc) const = 0;
    
    virtual void onMapUILayerUnitSelected(const UnderWorld::Core::Camp* camp) = 0;
    virtual void onMapUILayerUnitTouched(const UnderWorld::Core::Camp* camp) = 0;
    virtual void onMapUILayerUnitAdd(const UnderWorld::Core::Camp* camp) = 0;
    virtual void onMapUILayerUnitUpgrade(const UnderWorld::Core::Camp* camp, int idx) = 0;
    
    virtual void onMapUILayerTouchMoved(const UnderWorld::Core::Camp* camp, const Point& position) = 0;
    virtual void onMapUILayerTouchCancelled(const UnderWorld::Core::Camp* camp) = 0;
    virtual void onMapUILayerTouchEnded(const UnderWorld::Core::Camp* camp, const Point& position) = 0;
};

class MapUILayer
: public LayerColor
, public MapUITableObserver
, public CampInfoNodeObserver
, public MapUIUpgradeNodeObserver
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
    void updateRemainingTime(int time);
    void updateResources(float gold, float wood);
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
    
    // MapUITableObserver
    virtual void onMapUITableUnitAdd(const UnderWorld::Core::Camp* camp) override;
    virtual void onMapUITableUnitUpgrade(MapUIUnitNode* node) override;
    virtual void onMapUITableUnitTouchedBegan(const UnderWorld::Core::Camp* camp) override;
    virtual void onMapUITableUnitTouchedEnded(const UnderWorld::Core::Camp* camp) override;
    
    // CampInfoNodeObserver
    virtual void onCampInfoNodeClickedIcon(CampInfoNode* pSender, const UnderWorld::Core::UnitBase* unit) override;
    
    // MapUIUpgradeNodeObserver
    virtual void onMapUIUpgradeNodeClickedButton(const UnderWorld::Core::Camp* camp, int idx) override;
    
    void createUserInfo(bool left, const std::string& account);
    bool isGameOver() const;
    void onUnitTouched(const UnderWorld::Core::Camp* camp);
    void createTableViews();
    Node* createTableView(UnderWorld::Core::UnitClass uc, Node* parent);
    Rect getTableViewBoundingBox(UnderWorld::Core::UnitClass uc) const;
    void setHighlightedCamp(const UnderWorld::Core::Camp* camp, bool callback = false, bool ignoreProduction = false, bool check = true);
    void createUpgradeNode(const UnderWorld::Core::Camp* camp);
    
private:
    MapUILayerObserver *_observer;
    Point _tableViewPos;
    bool _isTouchingTableView;
    const UnderWorld::Core::Camp* _highlightedCamp;
    const UnderWorld::Core::Camp* _selectedCamp;
    // ======================== UI =============================
    std::map<UnderWorld::Core::UnitClass, MapUITable*> _tableViews;
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
