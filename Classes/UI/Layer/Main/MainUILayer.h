//
//  MainUILayer.h
//  Underworld_Client
//
//  Created by Andy on 15/12/9.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef MainUILayer_h
#define MainUILayer_h

#include "ChatLayer.h"

USING_NS_CC;

class ResourceNode;
class UserSimpleNode;

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
    virtual void onEnterTransitionDidFinish() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // ChatLayerObserver
    virtual void onChatLayerTouchedButton(ui::Widget::TouchEventType type) override;
    
    void updateAvatar(int idx);
    void updateExp();
    void updateResources();
    void moveChatLayer(bool folded, bool animated);
    void onChatLayerMoved(bool folded, const Point& point);
    
    FunctionButton* createButton(ButtonType type, const Point& point);
    void createButtonLeftward(ButtonType type, float& x, float y, float space);
    void createButtonUpward(ButtonType type, float x, float& y, float space);
    void onResourceButtonClicked(ResourceNode* node);
    void onFunctionButtonClicked(ButtonType type);
    
private:
    MainUILayerObserver *_observer;
    bool _isChatLayerFolded;
    bool _isChatLayerMoving;
    
    // UI
    ChatLayer* _chatLayer;
    UserSimpleNode* _userNode;
    ResourceNode* _staminaResourceNode;
    ResourceNode* _goldResourceNode;
    ResourceNode* _gemResourceNode;
};

#endif /* MainUILayer_h */
