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
#include "QuestNode.h"

USING_NS_CC;
USING_NS_CC_EXT;

class TabButton;
class ScrollBar;

class QuestLayerObserver
{
public:
    virtual ~QuestLayerObserver() {}
    virtual void onQuestLayerClosed() = 0;
};

class QuestLayer : public LayerColor, public TableViewDataSource, public QuestNodeObserver
{
public:
    static QuestLayer* create(int tabIndex);
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
    
    void addTabButton(Node* parent, const std::string& title, int tabIndex, const ui::Button::ccWidgetClickCallback& callback);
    void addTableView(int index);
    void switchTable(int index);
    
private:
    struct TabInfo {
        TableView* tableView;
        TabButton* tabButton;
    };
    
private:
    QuestLayerObserver *_observer;
    Size _tableViewMaxSize;
    Size _cellSize;
    ssize_t _cellsCount;
    std::unordered_map<int, TabInfo> _tabInfos;
    ScrollBar *_scrollBar;
    int _tabIndex;
};

#endif /* QuestLayer_h */
