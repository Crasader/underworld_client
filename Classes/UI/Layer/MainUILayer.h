//
//  MainUILayer.h
//  Underworld_Client
//
//  Created by Andy on 15/12/9.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef MainUILayer_h
#define MainUILayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class ResourceNode;

class MainUILayerObserver
{
public:
    virtual ~MainUILayerObserver() {}
};

class MainUILayer : public LayerColor
{
public:
    static MainUILayer* create();
    virtual ~MainUILayer();
    void registerObserver(MainUILayerObserver *observer);
    
protected:
    enum class ButtonType;
    class FunctionButton;
    
protected:
    MainUILayer();
    
    // LayerColor
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    void updateIcon();
    void updateExp();
    void updateResources();
    
    void onResourceButtonClicked(ResourceNode* node);
    void onFunctionButtonClicked(ButtonType type);
    
private:
    MainUILayerObserver *_observer;
    ui::Button* _iconButton;
    Label* _nameLabel;
    Label* _levelLabel;
    ProgressTimer* _expProgress;
    ResourceNode* _staminaResourceNode;
    ResourceNode* _goldResourceNode;
    ResourceNode* _gemResourceNode;
};

#endif /* MainUILayer_h */
