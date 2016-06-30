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
    static QuestLayer* create(QuestType type);
    virtual ~QuestLayer();
    void registerObserver(QuestLayerObserver *observer);
    
protected:
    QuestLayer();
    bool init(QuestType type);
    
    // TableViewDataSource
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override;
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(TableView *table) override;
    
    // table
    void createTableView(QuestType type);
    void refreshTable(TableView* table, bool reload);
    ssize_t getCellsCount(TableView *table) const;
    Size getCellSize() const;
    Rect getBoundingBox(Node* node) const;
    
    // buttons
    void createTabButtons(const Point& position);
    
    // functions
    void reloadAllCandidates();
    void reloadCandidates(QuestType type);
    void insertCandidate(QuestType type, const std::string& name);
    void removeCandidate(QuestType type, const std::string& name);
    QuestType getTableType(TableView* table) const;
    void setTableType(QuestType type);
    std::string getTableName(QuestType type) const;
    
private:
    QuestLayerObserver *_observer;
    
    // table
    std::map<QuestType, TableView*> _tables;
    QuestType _thisTableType;
    TableView* _thisTable;
    Size _nodeSize;
    Size _tableMaxSize;
    Point _tableBasePosition;
    
    std::map<QuestType, TabButton*> _tabButtons;
    
    // data
    std::map<QuestType, std::vector<std::string>> _candidates;
};

#endif /* QuestLayer_h */
