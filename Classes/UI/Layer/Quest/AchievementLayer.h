//
//  AchievementLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/6/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef AchievementLayer_h
#define AchievementLayer_h

#include "TableTemplate.h"
#include "AchievementNode.h"

class AchievementLayerObserver
{
public:
    virtual ~AchievementLayerObserver() {}
    virtual void onAchievementLayerClosed() = 0;
};

class AchievementLayer
: public LayerColor
, public TableTemplateObserver
, public AchievementNodeObserver
{
public:
    static AchievementLayer* create();
    virtual ~AchievementLayer();
    void registerObserver(AchievementLayerObserver *observer);
    
protected:
    AchievementLayer();
    virtual bool init() override;
    
    // TableTemplateObserver
    virtual Node* onTableTemplateCreateNodeModel(TableTemplate* tt) override;
    virtual void onTableTemplateUpdateNode(TableTemplate* tt, ssize_t idx, Node* node) override;
    virtual ssize_t numberOfNodesForTableTemplate(const TableTemplate* tt) override;
    
private:
    AchievementLayerObserver *_observer;
    TableTemplate* _tableTemplate;
};

#endif /* AchievementLayer_h */
