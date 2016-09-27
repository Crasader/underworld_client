//
//  GuildUISearchNode.h
//  Underworld_Client
//
//  Created by Andy on 16/9/8.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef GuildUISearchNode_h
#define GuildUISearchNode_h

#include "TableTemplate.h"
#include "ui/CocosGUI.h"
#include "GuildCell.h"

class GuildData;

class GuildUISearchNodeObserver
{
public:
    virtual ~GuildUISearchNodeObserver() {}
};

class GuildUISearchNode
: public Node
, public TableTemplateObserver
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
    
    // TableTemplateObserver
    virtual Node* onTableTemplateCreateNodeModel(TableTemplate* tt) override;
    virtual void onTableTemplateUpdateNode(TableTemplate* tt, ssize_t idx, Node* node) override;
    virtual ssize_t numberOfNodesForTableTemplate(const TableTemplate* tt) override;
    
    // EditBoxDelegate
    virtual void editBoxReturn(ui::EditBox* editBox) override;
    
    // GuildCellObserver
    virtual void onGuildCellTouched(const GuildData* data) override;
    
private:
    GuildUISearchNodeObserver* _observer;
    TableTemplate* _tableTemplate;
    std::vector<GuildData*> _guilds;
};

#endif /* GuildUISearchNode_h */
