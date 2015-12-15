//
//  QuestLayer.h
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef QuestLayer_h
#define QuestLayer_h

#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
USING_NS_CC_EXT;

class ScrollBar;

class QuestLayerObserver
{
public:
    virtual ~QuestLayerObserver() {}
};

class QuestLayer : public LayerColor, public TableViewDataSource
{
public:
    static QuestLayer* create(int levelId);
    virtual ~QuestLayer();
    void registerObserver(QuestLayerObserver *observer);
    
protected:
    QuestLayer();
    bool init(int levelId);
    
    // LayerColor
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // TableViewDataSource
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override;
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(TableView *table) override;
    
    void switchTable(int index);
    void setButtonSelected(int index);
    
private:
    QuestLayerObserver *_observer;
    Size _tableViewMaxSize;
    Size _cellSize;
    ssize_t _cellsCount;
    std::map<int, TableView*> _tableViews;
    std::vector<ui::Button*> _tabButtons;
    ScrollBar *_scrollBar;
    int _tabIndex;
};

#endif /* QuestLayer_h */
