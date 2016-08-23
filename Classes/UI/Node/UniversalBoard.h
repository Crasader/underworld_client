//
//  UniversalBoard.h
//  Underworld_Client
//
//  Created by Andy on 16/8/23.
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
    
    static UniversalBoard* create();
    virtual ~UniversalBoard();
    
    void setTitle(const std::string& title);
    void setExitCallback(const Callback& callback);
    
    Node* getSubNode() const;
    
protected:
    UniversalBoard();
    virtual bool init() override;
    
private:
    Node* _subNode;
    Label* _title;
    ui::Button* _exitButton;
    Callback _callback;
};

#endif /* UniversalBoard_h */
