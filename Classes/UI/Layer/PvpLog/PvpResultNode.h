//
//  PvpResultNode.h
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef PvpResultNode_h
#define PvpResultNode_h

#include "cocos2d.h"

USING_NS_CC;

class PvpResultNode : public Node
{
public:
    static PvpResultNode* create();
    virtual ~PvpResultNode();
    
    void setResult(bool win);
    
private:
    PvpResultNode();
    virtual bool init() override;
    
private:
    Sprite* _bg;
    Label* _result;
};

#endif /* PvpResultNode_h */
