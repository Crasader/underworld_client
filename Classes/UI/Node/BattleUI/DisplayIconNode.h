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
#include "UnitSimpleInfoNode.h"

USING_NS_CC;
USING_NS_CC_EXT;

namespace UnderWorld { namespace Core {
    class Camp;
    class Unit;
} }

class DisplayIconNodeObserver
{
public:
    virtual ~DisplayIconNodeObserver() {}
    virtual void onDisplayIconNodeTouchedEnded(const UnderWorld::Core::UnitBase* unit) = 0;
    virtual void onDisplayIconNodeChangedContentSize(const Size& lastSize, const Size& newSize) = 0;
};

class DisplayIconNode : public Node, public TableViewDataSource, public UnitSimpleInfoNodeObserver
{
public:
    static DisplayIconNode* create(bool scrollToLeft);
    virtual ~DisplayIconNode();
    
    virtual void setPosition(const Point& position) override;
    void registerObserver(DisplayIconNodeObserver *observer);
#if ENABLE_CAMP_INFO
    void insert(const std::vector<std::pair<const UnderWorld::Core::Camp*, const UnderWorld::Core::UnitBase*>>& units);
#else
    void insert(const std::vector<const UnderWorld::Core::UnitBase*>& units);
#endif
    void update();
    
protected:
    DisplayIconNode();
    bool init(bool scrollToLeft);
    void setTableViewSize(const Size& size);
    void reload();
    
    // TableViewDataSource
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override;
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(TableView *table) override;
    
    // UnitSimpleInfoNodeObserver
    virtual void onUnitSimpleInfoNodeTouched(Ref* pSender, ui::Widget::TouchEventType type, const UnderWorld::Core::UnitBase* unit) override;
    
private:
    DisplayIconNodeObserver *_observer;
    bool _scrollToLeft;
    Point _basePosition;
    Point _contentOffset;
    size_t _currentCount;
#if ENABLE_CAMP_INFO
    std::vector<const UnderWorld::Core::Camp*> _camps;
    std::map<const UnderWorld::Core::Camp*, const UnderWorld::Core::UnitBase*> _units;
#else
    std::vector<const UnderWorld::Core::UnitBase*> _unitBases;
#endif
    bool _touchInvalid;
    TableView *_tableView;
};

#endif /* DisplayIconNode_h */
