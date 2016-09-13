//
//  BillboardLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/9/13.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef BillboardLayer_h
#define BillboardLayer_h

#include "cocos2d.h"
#include "cocos-ext.h"
#include "BillboardCell.h"

USING_NS_CC;
USING_NS_CC_EXT;

class BillboardLayerObserver
{
public:
    virtual ~BillboardLayerObserver() {}
    virtual void onBillboardLayerSelected(ssize_t idx) = 0;
};

class BillboardLayer
: public LayerColor
, public TableViewDataSource
, public BillboardCellObserver
{
public:
    static BillboardLayer* create();
    virtual ~BillboardLayer();
    void registerObserver(BillboardLayerObserver *observer);
    
protected:
    BillboardLayer();
    
    // LayerColor
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // TableViewDataSource
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override;
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(TableView *table) override;
    
    // BillboardCellObserver
    virtual void onBillboardCellClicked(const BillboardData* data) override;
    
    // table
    void createTable(Node* parent);
    void refreshTable(bool reload);
    ssize_t getCellsCount() const;
    
    void updateRank(int rank);
    void updateTrophy(int trophy);
    
private:
    BillboardLayerObserver *_observer;
    TableView* _table;
    Size _nodeSize;
    Size _tableMaxSize;
    Point _tableBasePosition;
    ssize_t _selectedIdx;
    Label* _rank;
    Label* _trophy;
};

#endif /* BillboardLayer_h */
