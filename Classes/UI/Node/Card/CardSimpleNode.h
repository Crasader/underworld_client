//
//  CardSimpleNode.h
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardSimpleNode_h
#define CardSimpleNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class CardSimpleData;

class CardSimpleNode : public Node
{
public:
    static constexpr float Width = 53;
    static constexpr float Height = 68;
    typedef std::function<void()> Callback;
    
    static CardSimpleNode* create(const CardSimpleData* data);
    virtual ~CardSimpleNode();
    void update(const CardSimpleData* data);
    void setCallback(const Callback& callback);
    
private:
    CardSimpleNode();
    bool init(const CardSimpleData* data);
    
private:
    const CardSimpleData* _data;
    ui::Button* _icon;
    Callback _callback;
    bool _touchInvalid;
};

#endif /* CardSimpleNode_h */
