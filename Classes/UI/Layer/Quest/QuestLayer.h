//
//  QuestLayer.h
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef QuestLayer_h
#define QuestLayer_h

#include "TableTemplate.h"
#include "QuestNode.h"
#include "CocosGlobal.h"

class TabButton;

class QuestLayerObserver
{
public:
    virtual ~QuestLayerObserver() {}
    virtual void onQuestLayerClosed() = 0;
};

class QuestLayer
: public LayerColor
, public TableTemplateObserver
, public QuestNodeObserver
{
public:
    static QuestLayer* create(QuestType type);
    virtual ~QuestLayer();
    void registerObserver(QuestLayerObserver *observer);
    
protected:
    QuestLayer();
    bool init(QuestType type);
    
    // TableTemplateObserver
    virtual Node* onTableTemplateCreateNodeModel(TableTemplate* tt) override;
    virtual void onTableTemplateUpdateNode(TableTemplate* tt, ssize_t idx, Node* node) override;
    virtual ssize_t numberOfNodesForTableTemplate(const TableTemplate* tt) override;
    
    // UI
    void createTableTemplate(QuestType type);
    void createTabButtons(const Point& position);
    
    // functions
    QuestType getQuestType(const TableTemplate* tt) const;
    void setQuestType(QuestType type);
    std::string getTableName(QuestType type) const;
    
private:
    QuestLayerObserver *_observer;
    
    // table
    std::map<QuestType, TableTemplate*> _tableTemplats;
    QuestType _thisTableType;
    TableTemplate* _thisTableTemplat;
    
    std::map<QuestType, TabButton*> _tabButtons;
};

#endif /* QuestLayer_h */
