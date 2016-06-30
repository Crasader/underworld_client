//
//  AchievementNode.h
//  Underworld_Client
//
//  Created by Andy on 16/6/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef AchievementNode_h
#define AchievementNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class AchievementData;

class AchievementNodeObserver
{
public:
    virtual ~AchievementNodeObserver() {}
};

class AchievementNode: public Node
{
public:
    static AchievementNode* create(const AchievementData* data, ssize_t idx);
    virtual ~AchievementNode();
    void registerObserver(AchievementNodeObserver *observer);
    
    ssize_t getIdx() const;
    void update(const AchievementData* data, ssize_t idx);
    void updateProgress();
    
protected:
    AchievementNode();
    bool init(const AchievementData* data, ssize_t idx);
    
private:
    AchievementNodeObserver* _observer;
    const AchievementData* _data;
    ssize_t _idx;
    Label* _nameLabel;
    Label* _descriptionLabel;
    Label* _progressLabel;
    ui::Button* _button;
    Label* _buttonHintLabel;
};

#endif /* AchievementNode_h */
