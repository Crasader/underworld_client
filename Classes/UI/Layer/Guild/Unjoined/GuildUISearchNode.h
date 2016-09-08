//
//  GuildUISearchNode.h
//  Underworld_Client
//
//  Created by Andy on 16/9/8.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef GuildUISearchNode_h
#define GuildUISearchNode_h

#include "cocos2d.h"
#include "cocos-ext.h"
#include "ui/CocosGUI.h"
#include "GuildCell.h"

USING_NS_CC;
USING_NS_CC_EXT;

class GuildData;

class GuildUISearchNodeObserver
{
public:
    virtual ~GuildUISearchNodeObserver() {}
};

class GuildUISearchNode
: public Node
, public TableViewDataSource
, public ui::EditBoxDelegate
, public GuildCellObserver
{
public:
    static GuildUISearchNode* create(const Size& size);
    virtual ~GuildUISearchNode();
    void registerObserver(GuildUISearchNodeObserver *observer);
    
protected:
    GuildUISearchNode();
    bool init(const Size& size);
    
    // TableViewDataSource
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override;
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(TableView *table) override;
    
    // EditBoxDelegate
    virtual void editBoxReturn(ui::EditBox* editBox) override;
    
    // GuildCellObserver
    virtual void onGuildCellTouched(const GuildData* data) override;
    
    // table
    void createTable();
    void refreshTable(bool reload);
    ssize_t getCellsCount() const;
    
private:
    GuildUISearchNodeObserver* _observer;
    TableView* _table;
    Size _nodeSize;
    Size _tableMaxSize;
    Point _tableBasePosition;
    std::vector<GuildData*> _guilds;
};

#endif /* GuildUISearchNode_h */
