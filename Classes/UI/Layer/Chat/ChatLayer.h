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
#include "ChatTCPClientObserver.h"

USING_NS_CC;
USING_NS_CC_EXT;

class ScrollBar;
class TabButton;
class ChatData;

class ChatLayerObserver
{
public:
    virtual ~ChatLayerObserver() {}
    virtual void onChatLayerTouchedButton(ui::Button* button, ui::Widget::TouchEventType type) = 0;
};

class ChatLayer
: public LayerColor
, public TableViewDataSource
, public ui::EditBoxDelegate
, public ChatNodeObserver
, public NoticeNodeObserver
, public ChatTCPClientObserver
{
public:
    static ChatLayer* create();
    virtual ~ChatLayer();
    void registerObserver(ChatLayerObserver *observer);
    void setButtonStatus(bool fold);
    void setFocus(bool focus);
    
protected:
    class TableNode;
    class EditBoxNode;
    
    ChatLayer();
    
    // LayerColor
    virtual bool init() override;
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
    virtual void onChatNodeTouchedAvatar(const ChatData* data) override;
    
    // NoticeNodeObserver
    virtual void onNoticeNodeGet(const ChatData* data) override;
    virtual void onNoticeNodeDelete(const ChatData* data) override;
    
    // ChatTCPClientObserver
    virtual void onChatTCPClientConnected() override;
    virtual void onChatTCPClientReconnected() override;
    virtual void onChatTCPClientSent(const ChatData* data) override;
    virtual void onChatTCPClientReceived(const ChatData* data) override;
    virtual void onChatTCPClientError(int code) override;
    
    // table
    void createTableNode(ChatType type, float height, const Point& topLeft);
    ssize_t getCellsCount(TableView* table) const;
    Rect getBoundingBox(Node* node) const;
    
    // functions
    void createTabButtons(const Vec2& edge);
    Size getCellSize(ChatType type, size_t idx);
    Node* createCellNode(ChatType type, size_t idx);
    void updateCellNode(Node* node, ChatType type, size_t idx);
    
    size_t getDataCount(ChatType type) const;
    ChatType getTableType(TableView* table) const;
    void setTableType(ChatType type);
    std::string getTableName(ChatType type) const;
    
    void sendMessage();
    
private:
    ChatLayerObserver *_observer;
    
    // UI
    Node* _background;
    Sprite* _buttonIcon;
    std::map<ChatType, TableNode*> _tableNodes;
    std::unordered_map<TableView*, TableNode*> _tableMappings;
    ChatType _thisTableType;
    TableNode* _thisTableNode;
    EditBoxNode* _editBoxNode;
    std::map<ChatType, TabButton*> _tabButtons;
    ScrollBar* _scrollBar;
};

#endif /* ChatLayer_h */
