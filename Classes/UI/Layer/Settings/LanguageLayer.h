//
//  LanguageLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/7/25.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef LanguageLayer_h
#define LanguageLayer_h

#include "TableTemplate.h"
#include "LanguageNode.h"
#include "LanguageConfirmationLayer.h"

class LanguageLayerObserver
{
public:
    virtual ~LanguageLayerObserver() {}
    virtual void onLanguageLayerSelected(ssize_t idx) = 0;
};

class LanguageLayer
: public LayerColor
, public TableTemplateObserver
, public LanguageNodeObserver
, public LanguageConfirmationLayerObserver
{
public:
    static LanguageLayer* create(const Size& size);
    virtual ~LanguageLayer();
    void registerObserver(LanguageLayerObserver *observer);
    
protected:
    LanguageLayer();
    
    // LayerColor
    bool init(const Size& size);
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // TableTemplateObserver
    virtual Node* onTableTemplateCreateNodeModel(TableTemplate* tt) override;
    virtual void onTableTemplateUpdateNode(TableTemplate* tt, ssize_t idx, Node* node) override;
    virtual ssize_t numberOfNodesForTableTemplate(const TableTemplate* tt) override;
    
    // LanguageNodeObserver
    virtual void onLanguageNodeSelected(ssize_t idx) override;
    
    // LanguageConfirmationLayerObserver
    virtual void onLanguageConfirmationLayerConfirm(Node* pSender, ssize_t idx) override;
    
private:
    LanguageLayerObserver *_observer;
    TableTemplate* _tableTemplate;
    ssize_t _selectedIdx;
};

#endif /* LanguageLayer_h */
