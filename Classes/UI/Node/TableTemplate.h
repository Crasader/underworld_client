//
//  TableTemplate.h
//  Underworld_Client
//
//  Created by Andy on 16/9/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef TableTemplate_h
#define TableTemplate_h

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class TableTemplate;

class TableTemplateObserver
{
public:
    virtual ~TableTemplateObserver() {}
    virtual Node* onTableTemplateCreateNodeModel(TableTemplate* tt) = 0;
    virtual void onTableTemplateUpdateNode(TableTemplate* tt, ssize_t idx, Node* node) = 0;
    virtual ssize_t numberOfNodesForTableTemplate(const TableTemplate* tt) = 0;
};

class TableTemplate : public TableViewDataSource
{
public:
    static const float DefaultGap;
    
    TableTemplate(Node* parent, const Point& position, bool horizontal, const Size& size, int elementCountPerCell, float gap, TableTemplateObserver* observer);
    virtual ~TableTemplate();
    
    enum class ContentOffsetType { BEGIN, END };
    
    void setTag(int tag);
    void setContentOffsetType(ContentOffsetType type);
    void refreshTable(bool reload);
    void refreshTable(float length, bool reload, bool keepContentOffset);
    
    int getTag() const;
    TableView* getTableView() const;
    
private:
    // TableViewDataSource
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override;
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(TableView *table) override;
    
    Vec2 getContentOffset(ContentOffsetType type) const;
    ssize_t getCellsCount() const;
    
private:
    int _tag;
    TableView* _table;
    bool _isHorizontal;
    Size _nodeSize;
    Size _cellSize;
    Size _maxSize;
    Point _basePosition;
    
    int _elementCountPerCell;
    float _fixedGap;
    Size _fixedGapSize;
    Size _unfixedGapSize;
    TableTemplateObserver *_observer;
};

#endif /* TableTemplate_h */
