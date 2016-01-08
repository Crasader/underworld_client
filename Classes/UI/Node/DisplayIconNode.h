//
//  DisplayIconNode.h
//  Underworld_Client
//
//  Created by Andy on 16/1/8.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef DisplayIconNode_h
#define DisplayIconNode_h

#include "cocos2d.h"
#include "cocos-ext.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
USING_NS_CC_EXT;

class DisplayIconNodeObserver
{
public:
    virtual ~DisplayIconNodeObserver() {}
    virtual void onDisplayIconNodeTouchedEnded(int unitId) = 0;
};

class DisplayIconNode : public Node, public TableViewDataSource
{
public:
    static DisplayIconNode* create(bool scrollToLeft, const Point& position);
    virtual ~DisplayIconNode();
    
    void registerObserver(DisplayIconNodeObserver *observer);
    void addIcon(int unitId);
    
protected:
    DisplayIconNode();
    bool init(bool scrollToLeft, const Point& position);
    void reload();
    
    // TableViewDataSource
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override;
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(TableView *table) override;
    
private:
    DisplayIconNodeObserver *_observer;
    bool _scrollToLeft;
    Point _basePosition;
    Point _contentOffset;
    size_t _currentCount;
    std::vector<int> _unitIds;
    bool _touchInvalid;
    TableView *_tableView;
};

#endif /* DisplayIconNode_h */
