//
//  XButton.h
//  Underworld_Client
//
//  Created by Andy on 16/7/27.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef XButton_h
#define XButton_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class XButton : public ui::Button
{
public:
    enum class BSize {
        Big,
        Small,
    };
    
    enum class BType {
        Blue,
        Red,
        Green,
        Purple
    };
    
public:
    static XButton* create(BSize size, BType type);
    static XButton* createReturnButton(Node* parent, const Vec2& offset);
    virtual ~XButton();
    
    void setType(BType type);
    
protected:
    XButton();
    bool init(BSize size, BType type);
    
private:
    BSize _bSize;
    BType _bType;
};

#endif /* XButton_h */
