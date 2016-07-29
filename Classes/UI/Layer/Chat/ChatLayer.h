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
#include "CocosGlobal.h"
#include "ChatNode.h"
#include "NoticeNode.h"

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
    void createTableNode(ChatType type, float height, const Point& topLeft);
    ssize_t getCellsCount(TableView* table) const;
    Rect getBoundingBox(Node* node) const;
    
    // functions
    void createTabButtons(const Vec2& edge);
    Size getCellSize(ChatType type, size_t idx);
    Node* createCellNode(ChatType type, size_t idx);
    
    size_t getDataSize(ChatType type) const;
    ChatType getTableType(TableView* table) const;
    void setTableType(ChatType type);
    std::string getTableName(ChatType type) const;
    
private:
    ChatLayerObserver *_observer;
    bool _folder;
    bool _isFolding;
    
    // UI
    Node* _background;
    ui::Button* _button;
    Sprite* _buttonIcon;
    std::map<ChatType, TableNode*> _tableNodes;
    std::unordered_map<const TableView*, TableNode*> _tableMappings;
    std::map<ChatType, float> _tableHeight;
    ChatType _thisTableType;
    TableNode* _thisTableNode;
    EditBoxNode* _editBoxNode;
    std::map<ChatType, TabButton*> _tabButtons;
    ScrollBar* _scrollBar;
};

#endif /* ChatLayer_h */
