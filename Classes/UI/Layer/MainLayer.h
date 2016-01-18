//
//  MainLayer.h
//  Underworld_Client
//
//  Created by Andy on 15/12/9.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef MainLayer_h
#define MainLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
class MainUILayer;

class MainLayerObserver
{
public:
    virtual ~MainLayerObserver() {}
};

class MainLayer : public LayerColor
{
public:
    static Scene* createScene();
    virtual ~MainLayer();
    void registerObserver(MainLayerObserver *observer);
    
protected:
    static MainLayer* create();
    MainLayer();
    
    // LayerColor
    virtual bool init() override;
    virtual void onEnter() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    void addLevelButtons();
    
private:
    MainLayerObserver *_observer;
    MainUILayer *_uiLayer;
    Node *_mainNode;
    ui::ScrollView *_scrollView;
    bool _touchInvalid;
};

#endif /* MainLayer_h */
