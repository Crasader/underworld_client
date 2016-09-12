//
//  TestLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/8/4.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef TestLayer_h
#define TestLayer_h

#include "cocos2d.h"

USING_NS_CC;

class TestLayer : public LayerColor
{
public:
    static TestLayer* getInstance();
    static void purge();
    void show(Node* parent);
    
protected:
    typedef std::function<void(size_t, bool)> Callback;
    
    TestLayer();
    virtual ~TestLayer();
    bool init(const std::function<void(Ref*)>& callback);
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    MenuItem* createMenuItem(const Callback& callback);
    void onMenuItemClicked(size_t idx, bool initialized);
    
private:
    Node* _bg;
    std::vector<Label*> _labels;
};

#endif /* TestLayer_h */
