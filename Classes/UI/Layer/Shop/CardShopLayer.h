//
//  CardShopLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/9/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardShopLayer_h
#define CardShopLayer_h

#include "cocos-ext.h"
#include "BaseCard.h"

USING_NS_CC_EXT;

class CardShopLayerObserver
{
public:
    virtual ~CardShopLayerObserver() {}
};

class CardShopLayer
: public LayerColor
, public TableViewDataSource
, public BaseCardObserver
{
public:
    static CardShopLayer* create();
    virtual ~CardShopLayer();
    void registerObserver(CardShopLayerObserver *observer);
    
protected:
    CardShopLayer();
    
    // LayerColor
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // TableViewDataSource
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override;
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(TableView *table) override;
    
    // BaseCardObserver
    virtual void onBaseCardClicked(BaseCard* pSender) override;
    virtual void onBaseCardClickedResourceButton(BaseCard* pSender) override;
    
    // table
    void createTable(Node* parent);
    void refreshTable(bool reload);
    ssize_t getCellsCount() const;
    
private:
    CardShopLayerObserver *_observer;
    TableView* _table;
    Size _nodeSize;
    Size _cellSize;
    Size _tableMaxSize;
    Point _tableBasePosition;
    float _tableNodeGapX;
};

#endif /* CardShopLayer_h */
