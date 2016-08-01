//
//  LanguageLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/7/25.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef LanguageLayer_h
#define LanguageLayer_h

#include "cocos2d.h"
#include "cocos-ext.h"
#include "LanguageNode.h"

USING_NS_CC;
USING_NS_CC_EXT;

class LanguageLayerObserver
{
public:
    virtual ~LanguageLayerObserver() {}
    virtual void onLanguageLayerReturn() = 0;
    virtual void onLanguageLayerClosed() = 0;
};

class LanguageLayer
: public LayerColor
, public TableViewDataSource
, public LanguageNodeObserver
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
    
    // TableViewDataSource
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override;
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(TableView *table) override;
    
    // LanguageNodeObserver
    
    // table
    void createTable();
    void refreshTable(bool reload);
    ssize_t getCellsCount() const;
    Size getCellSize() const;
    Rect getBoundingBox(Node* node) const;
    
private:
    LanguageLayerObserver *_observer;
    TableView* _table;
    Size _nodeSize;
    Size _tableMaxSize;
    Point _tableBasePosition;
    std::vector<std::string> _languages;
};

#endif /* LanguageLayer_h */
