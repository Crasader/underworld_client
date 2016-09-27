//
//  BillboardLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/9/13.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef BillboardLayer_h
#define BillboardLayer_h

#include "TableTemplate.h"
#include "BillboardCell.h"

class BillboardLayerObserver
{
public:
    virtual ~BillboardLayerObserver() {}
    virtual void onBillboardLayerSelected(ssize_t idx) = 0;
};

class BillboardLayer
: public LayerColor
, public TableTemplateObserver
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
    
    // TableTemplateObserver
    virtual Node* onTableTemplateCreateNodeModel(TableTemplate* tt) override;
    virtual void onTableTemplateUpdateNode(TableTemplate* tt, ssize_t idx, Node* node) override;
    virtual ssize_t numberOfNodesForTableTemplate(const TableTemplate* tt) override;
    
    // BillboardCellObserver
    virtual void onBillboardCellClicked(const BillboardData* data) override;
    
    void updateRank(int rank);
    void updateTrophy(int trophy);
    
private:
    BillboardLayerObserver *_observer;
    TableTemplate* _tableTemplate;
    Label* _rank;
    Label* _trophy;
};

#endif /* BillboardLayer_h */
