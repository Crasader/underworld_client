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
    static AchievementNode* create();
    virtual ~AchievementNode();
    void registerObserver(AchievementNodeObserver *observer);
    
    void update(const AchievementData* data);
    void updateProgress();
    
protected:
    AchievementNode();
    virtual bool init() override;
    
private:
    AchievementNodeObserver* _observer;
    const AchievementData* _data;
    Label* _nameLabel;
    Label* _descriptionLabel;
    Label* _progressLabel;
    ui::Button* _button;
    Label* _buttonHintLabel;
};

#endif /* AchievementNode_h */
