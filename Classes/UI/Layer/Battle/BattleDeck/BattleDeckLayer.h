//
//  BattleDeckLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/2/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef BattleDeckLayer_h
#define BattleDeckLayer_h

#include "AbstractUILayer.h"
#include "cocos-ext.h"
#include "BattleDeckUnitNode.h"
#include "UnitType.h"

USING_NS_CC;
USING_NS_CC_EXT;

class BattleDeckUnitInfoNode;

class BattleDeckLayerObserver
{
public:
    virtual ~BattleDeckLayerObserver() {}
};

class BattleDeckLayer : public AbstractUILayer, public TableViewDelegate, public TableViewDataSource, public BattleDeckUnitNodeObserver
{
public:
    static BattleDeckLayer* create();
    virtual ~BattleDeckLayer();
    void registerObserver(BattleDeckLayerObserver *observer);
    
protected:
    BattleDeckLayer();
    virtual bool init() override;
    
    // LayerColor
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchMoved(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // TableViewDelegate
    virtual void tableCellTouched(TableView* table, TableViewCell* cell) override;
    
    // TableViewDataSource
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override;
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(TableView *table) override;
    
    // BattleDeckUnitNodeObserver
    virtual void onBattleDeckUnitNodeTouchedBegan(const UnderWorld::Core::Camp* camp) override;
    virtual void onBattleDeckUnitNodeTouchedEnded(const UnderWorld::Core::Camp* camp, bool isValid) override;
    virtual void onBattleDeckUnitNodeTouchedCanceled(const UnderWorld::Core::Camp* camp) override;
    
    void createTableViews(float width);
    Node* createTableView(UnderWorld::Core::UnitClass uc, float width);
    UnderWorld::Core::UnitClass getUnitClass(TableView* table) const;
    ssize_t getCellsCount(TableView* table) const;
    ssize_t getCellsCount(UnderWorld::Core::UnitClass uc) const;
    Rect getTableViewBoundingBox(TableView* table) const;
    
private:
    BattleDeckLayerObserver *_observer;
    Point _tableViewPos;
    Size _cellSize;
    std::pair<TableView*, ssize_t> _selectedCampIdx;
    BattleDeckUnitInfoNode* _infoNode;
    std::map<UnderWorld::Core::UnitClass, TableView*> _tableViews;
    Label* _selectedCardsLabel;
    std::vector<Sprite*> _cardDecks;
};

#endif /* BattleDeckLayer_h */
