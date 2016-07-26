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
#include "cocos-ext.h"
#include "ui/CocosGUI.h"
#include "ChatNode.h"

USING_NS_CC;
USING_NS_CC_EXT;

class ScrollBar;
class TabButton;
class ChatData;

class ChatLayerObserver
{
public:
    virtual ~ChatLayerObserver() {}
};

enum class ChatTableType {
    None,
    Talk,
    Mail,
    Notice,
};

class ChatLayer
: public LayerColor
, public TableViewDataSource
, public ui::EditBoxDelegate
, public ChatNodeObserver
{
public:
    static ChatLayer* create();
    virtual ~ChatLayer();
    void registerObserver(ChatLayerObserver *observer);
    
protected:
    ChatLayer();
    virtual bool init() override;
    
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
    
    // ChatNodeObserver
    
    // table
    void createTableView(ChatTableType type);
    void refreshTable(TableView* table, bool reload);
    ssize_t getCellsCount(TableView *table) const;
    Size getCellSize() const;
    Rect getBoundingBox(Node* node) const;
    
    // functions
    void createTabButtons(const Point& position);
    ChatNode* createChatNode(const ChatData* data);
    void createEditBox();
    ChatTableType getTableType(TableView* table) const;
    void setTableType(ChatTableType type);
    std::string getTableName(ChatTableType type) const;
    
private:
    ChatLayerObserver *_observer;
    
    // table
    std::map<ChatTableType, TableView*> _tables;
    ChatTableType _thisTableType;
    TableView* _thisTable;
    Size _nodeSize;
    Size _tableMaxSize;
    Point _tableBasePosition;
    
    Sprite* _editBoxBg;
    ui::EditBox* _editBox;
    
    std::map<ChatTableType, TabButton*> _tabButtons;
    ScrollBar *_scrollBar;
    
    // data
    std::map<ChatTableType, std::vector<ChatData*>> _messages;
};

#endif /* ChatLayer_h */
