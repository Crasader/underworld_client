//
//  TrophyGapNode.h
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef TrophyGapNode_h
#define TrophyGapNode_h

#include "cocos2d.h"

USING_NS_CC;

class TrophyGapNode : public Node
{
public:
    static TrophyGapNode* create();
    virtual ~TrophyGapNode();
    
    void setCount(int count);
    
private:
    TrophyGapNode();
    virtual bool init() override;
    
private:
    Label* _trophy;
};

#endif /* TrophyGapNode_h */
