//
//  UniversalBoard.h
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef UniversalBoard_h
#define UniversalBoard_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class UniversalBoard: public Sprite
{
public:
    typedef std::function<void()> Callback;
    
    static UniversalBoard* create(const std::string& file);
    virtual ~UniversalBoard();
    
    void setTitle(const std::string& title);
    void setExitButtonVisible(bool visible);
    void setExitCallback(const Callback& callback);
    
protected:
    UniversalBoard();
    bool init(const std::string& file);
    
protected:
    Label* _title;
    ui::Button* _exitButton;
    Callback _exitCallback;
};

#endif /* UniversalBoard_h */
