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
};

class LanguageNode: public Node
{
public:
    static LanguageNode* create(const std::string& name);
    virtual ~LanguageNode();
    void registerObserver(LanguageNodeObserver *observer);
    void tick(bool ticked);
    void update(const std::string& name);
    
protected:
    LanguageNode();
    bool init(const std::string& name);
    
private:
    LanguageNodeObserver* _observer;
    ui::Button* _button;
    Node* _tick;
};

#endif /* LanguageNode_h */
