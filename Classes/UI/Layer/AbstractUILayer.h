//
//  AbstractUILayer.h
//  Underworld_Client
//
//  Created by Andy on 16/2/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef AbstractUILayer_h
#define AbstractUILayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

class AbstractUILayer : public LayerColor
{
protected:
    AbstractUILayer();
    virtual ~AbstractUILayer() = 0;
    bool init(const std::string& title);
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
protected:
    Sprite *_background;
    Button *_closeButton;
};

#endif /* AbstractUILayer_h */
