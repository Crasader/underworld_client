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
    MainUILayer();
    
    // LayerColor
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    void updateButtonTextures(ui::Button* button, int childTag, const std::string& normal, const std::string& touched);
    void setButtonSelected(ui::Button* button);
    void updateIcon();
    void updateResources();
    
private:
    MainUILayerObserver *_observer;
    ui::Button* _iconButton;
    Label* _nameLabel;
    LabelAtlas* _levelLabel;
    ui::LoadingBar *_expProgressBar;
    LabelAtlas* _boltLabel;
    LabelAtlas* _coinLabel;
    LabelAtlas* _gemLabel;
    ui::Button* _pvpButton;
    ui::Button* _bagButton;
    ui::Button* _questButton;
    ui::Button* _optionButton;
    ui::Button* _guildButton;
    ui::Button* _armyButton;
    ResourceNode* _jadeResourceNode;
    ResourceNode* _goldResourceNode;
    ResourceNode* _gemResourceNode;
};

#endif /* MainUILayer_h */
