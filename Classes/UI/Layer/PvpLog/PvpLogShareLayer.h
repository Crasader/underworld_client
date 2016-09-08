//
//  PvpLogShareLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/8/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef PvpLogShareLayer_h
#define PvpLogShareLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class XEditBox;

class PvpLogShareLayerObserver
{
public:
    virtual ~PvpLogShareLayerObserver() {}
    virtual void onPvpLogShareLayerClickedExitButton(Node* pSender) = 0;
    virtual void onPvpLogShareLayerClickedShareButton(Node* pSender, const std::string& msg) = 0;
};

class PvpLogShareLayer
: public LayerColor
, public ui::EditBoxDelegate
{
public:
    static PvpLogShareLayer* create();
    virtual ~PvpLogShareLayer();
    void registerObserver(PvpLogShareLayerObserver *observer);
    
protected:
    PvpLogShareLayer();
    
    // LayerColor
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // EditBoxDelegate
    virtual void editBoxReturn(ui::EditBox* editBox) override;
    
private:
    PvpLogShareLayerObserver *_observer;
    Sprite* _background;
    XEditBox* _editBox;
};

#endif /* PvpLogShareLayer_h */
