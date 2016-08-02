//
//  TrophyNode.h
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef TrophyNode_h
#define TrophyNode_h

#include "cocos2d.h"

USING_NS_CC;

class TrophyNode : public Node
{
public:
    static TrophyNode* create();
    virtual ~TrophyNode();
    
    void setCount(int count);
    
private:
    TrophyNode();
    virtual bool init() override;
    
private:
    Label* _trophy;
};

#endif /* TrophyNode_h */
