//
//  XEditBox.h
//  Underworld_Client
//
//  Created by Andy on 16/9/8.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef XEditBox_h
#define XEditBox_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class XEditBox : public ui::EditBox
{
public:
    static XEditBox* create(const Size& size);
    virtual ~XEditBox();
    
private:
    XEditBox();
    bool init(const Size& size);
};

#endif /* XEditBox_h */
