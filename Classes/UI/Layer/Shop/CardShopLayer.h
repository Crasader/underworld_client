//
//  CardShopLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/9/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardShopLayer_h
#define CardShopLayer_h

#include "TableTemplate.h"
#include "BaseCard.h"

class CardShopLayerObserver
{
public:
    virtual ~CardShopLayerObserver() {}
};

class CardShopLayer
: public LayerColor
, public TableTemplateObserver
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
    
    // TableTemplateObserver
    virtual Node* onTableTemplateCreateNodeModel(TableTemplate* tt) override;
    virtual void onTableTemplateUpdateNode(TableTemplate* tt, ssize_t idx, Node* node) override;
    virtual ssize_t numberOfNodesForTableTemplate(const TableTemplate* tt) override;
    
    // BaseCardObserver
    virtual void onBaseCardClicked(BaseCard* pSender) override;
    virtual void onBaseCardClickedResourceButton(BaseCard* pSender) override;
    
private:
    CardShopLayerObserver *_observer;
    TableTemplate* _tableTemplate;
};

#endif /* CardShopLayer_h */
