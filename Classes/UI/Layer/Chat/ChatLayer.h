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
#include "NoticeNode.h"

USING_NS_CC;
USING_NS_CC_EXT;

class ScrollBar;
class TabButton;
class ChatData;
class NoticeData;

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
, public NoticeNodeObserver
{
public:
    static ChatLayer* create();
    virtual ~ChatLayer();
    void registerObserver(ChatLayerObserver *observer);
    
protected:
    class TableNode;
    class EditBoxNode;
    
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
    
    // NoticeNodeObserver
    
    // table
    void createTableNode(ChatTableType type, float height, const Point& topLeft);
    ssize_t getCellsCount(TableView* table) const;
    Rect getBoundingBox(Node* node) const;
    
    // functions
    void createTabButtons(const Vec2& edge);
    Node* createCellNode(ChatTableType type, size_t idx);
    void createEditBox();
    
    size_t getDataSize(ChatTableType type) const;
    ChatTableType getTableType(TableView* table) const;
    void setTableType(ChatTableType type);
    std::string getTableName(ChatTableType type) const;
    
private:
    ChatLayerObserver *_observer;
    
    Node* _background;
    // table
    std::map<ChatTableType, TableNode*> _tableNodes;
    std::unordered_map<TableView*, TableNode*> _tableMappings;
    std::map<ChatTableType, float> _tableHeight;
    ChatTableType _thisTableType;
    TableNode* _thisTableNode;
    
    EditBoxNode* _editBoxNode;
    
    std::map<ChatTableType, TabButton*> _tabButtons;
    ScrollBar* _scrollBar;
    
    // data
    std::vector<ChatData*> _messages;
    std::vector<NoticeData*> _mails;
    std::vector<NoticeData*> _notices;
};

#endif /* ChatLayer_h */
