//
//  LanguageNode.h
//  Underworld_Client
//
//  Created by Andy on 16/7/25.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef LanguageNode_h
#define LanguageNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class LanguageNodeObserver
{
public:
    virtual ~LanguageNodeObserver() {}
    virtual void onLanguageNodeSelected(ssize_t idx) = 0;
};

class LanguageNode: public Node
{
public:
    static LanguageNode* create();
    virtual ~LanguageNode();
    void registerObserver(LanguageNodeObserver *observer);
    void tick(bool ticked);
    void update(const std::string& name);
    void setIdx(ssize_t idx);
    
protected:
    LanguageNode();
    virtual bool init() override;
    
private:
    LanguageNodeObserver* _observer;
    ui::Button* _button;
    Node* _tick;
    ssize_t _idx;
    bool _touchInvalid;
};

#endif /* LanguageNode_h */
