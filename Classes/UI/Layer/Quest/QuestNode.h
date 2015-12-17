//
//  QuestNode.h
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef QuestNode_h
#define QuestNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "CocosGlobal.h"

USING_NS_CC;

class QuestData;

class QuestNodeObserver
{
public:
    virtual ~QuestNodeObserver() {}
};

class QuestNode: public Node
{
public:
    static QuestNode* create(const QuestData* data, ssize_t idx);
    virtual ~QuestNode();
    void registerObserver(QuestNodeObserver *observer);
    
    ssize_t getIdx() const;
    void update(const QuestData* data, ssize_t idx);
    void updateProgress();
    
protected:
    QuestNode();
    bool init(const QuestData* data, ssize_t idx);
    
private:
    QuestNodeObserver* _observer;
    const QuestData* _data;
    ssize_t _idx;
    Label* _nameLabel;
    Label* _descriptionLabel;
    Label* _progressLabel;
    ui::Button* _button;
    Label* _buttonHintLabel;
};

#endif /* QuestNode_h */
