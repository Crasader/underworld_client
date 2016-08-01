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
#include "ChatLayer.h"

USING_NS_CC;

class ResourceNode;

class MainUILayerObserver
{
public:
    virtual ~MainUILayerObserver() {}
};

class MainUILayer
: public LayerColor
, public ChatLayerObserver
{
public:
    static MainUILayer* create();
    virtual ~MainUILayer();
    void registerObserver(MainUILayerObserver *observer);
    
protected:
    enum class ButtonType;
    class FunctionButton;
    
    MainUILayer();
    
    // LayerColor
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // ChatLayerObserver
    virtual void onChatLayerClickedButton() override;
    
    void updateAvatar();
    void updateExp();
    void updateResources();
    void moveChatLayer(bool folded, bool animated);
    void onChatLayerMoved(bool folded);
    
    void onResourceButtonClicked(ResourceNode* node);
    void onFunctionButtonClicked(ButtonType type);
    
private:
    MainUILayerObserver *_observer;
    bool _isChatLayerFolded;
    bool _isChatLayerMoving;
    
    // UI
    ChatLayer* _chatLayer;
    ui::Button* _avatarButton;
    Label* _nameLabel;
    Label* _levelLabel;
    ProgressTimer* _expProgress;
    ResourceNode* _staminaResourceNode;
    ResourceNode* _goldResourceNode;
    ResourceNode* _gemResourceNode;
};

#endif /* MainUILayer_h */
