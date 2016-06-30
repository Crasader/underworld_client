//
//  AchievementLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/6/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef AchievementLayer_h
#define AchievementLayer_h

#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "AchievementNode.h"

USING_NS_CC;
USING_NS_CC_EXT;

class ScrollBar;

class AchievementLayerObserver
{
public:
    virtual ~AchievementLayerObserver() {}
    virtual void onAchievementLayerClosed() = 0;
};

class AchievementLayer : public LayerColor, public TableViewDataSource, public AchievementNodeObserver
{
public:
    static AchievementLayer* create();
    virtual ~AchievementLayer();
    void registerObserver(AchievementLayerObserver *observer);
    
protected:
    AchievementLayer();
    virtual bool init() override;
    
    // TableViewDataSource
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override;
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(TableView *table) override;
    
    // table
    void createTableView();
    void refreshTable(TableView* table, bool reload);
    ssize_t getCellsCount(TableView *table) const;
    Size getCellSize() const;
    Rect getBoundingBox(Node* node) const;
    
    // functions
    void reloadAllCandidates();
    void insertCandidate(const std::string& name);
    void removeCandidate(const std::string& name);
    
private:
    AchievementLayerObserver *_observer;
    
    // table
    TableView* _table;
    TableView* _thisTable;
    Size _nodeSize;
    Size _tableMaxSize;
    Point _tableBasePosition;
    
    // data
    std::vector<std::string> _candidates;
};

#endif /* AchievementLayer_h */
