//
//  ChatLayer.h
//  Underworld_Client
//
//  Created by Andy on 15/12/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ChatLayer_h
#define ChatLayer_h

#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
USING_NS_CC_EXT;

class ScrollBar;

class ChatLayerObserver
{
public:
    virtual ~ChatLayerObserver() {}
};

class ChatLayer : public LayerColor, public TableViewDataSource, public TableViewDelegate, public ui::EditBoxDelegate
{
public:
    static ChatLayer* create(int levelId);
    virtual ~ChatLayer();
    void registerObserver(ChatLayerObserver *observer);
    
protected:
    ChatLayer();
    bool init(int levelId);
    
    // LayerColor
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // TableViewDataSource
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override;
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(TableView *table) override;
    
    // EditBoxDelegate
    virtual void editBoxTextChanged(ui::EditBox* editBox, const std::string& text) override;
    virtual void editBoxReturn(ui::EditBox* editBox) override;
    
    void addEditBox();
    void switchTable(int index);
    void setButtonSelected(int index);
    
private:
    ChatLayerObserver *_observer;
    Size _tableViewMaxSize;
    Size _cellSize;
    ssize_t _cellsCount;
    Sprite* _editBoxBg;
    ui::EditBox* _editBox;
    std::map<int, TableView*> _tableViews;
    std::vector<ui::Button*> _tabButtons;
    ScrollBar *_scrollBar;
    int _tabIndex;
};

#endif /* ChatLayer_h */
