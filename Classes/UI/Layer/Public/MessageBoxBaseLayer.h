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

class XButton;
class SmallBoard;

class MessageBoxBaseLayer : public LayerColor
{
protected:
    MessageBoxBaseLayer();
    virtual ~MessageBoxBaseLayer() = 0;
    virtual bool init() override;
    
protected:
    SmallBoard* _board;
    XButton* _confirmButton;
    XButton* _cancelButton;
    Point _confirmButtonPosition;
    Point _cancelButtonPosition;
};

#endif /* MessageBoxBaseLayer_h */
