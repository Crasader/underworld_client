//
//  MessageBoxBaseLayer.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef MessageBoxBaseLayer_h
#define MessageBoxBaseLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

class MessageBoxBaseLayer : public LayerColor
{
protected:
    MessageBoxBaseLayer();
    virtual ~MessageBoxBaseLayer() = 0;
    virtual bool init() override;
    virtual void onEnter() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
protected:
    Sprite *_background;
    Button *_confirmButton;
    Button *_cancelButton;
    Point _confirmButtonPosition;
    Point _cancelButtonPosition;
    Label *_confirmLabel;
    Label *_cancelLabel;
};

#endif /* MessageBoxBaseLayer_h */
