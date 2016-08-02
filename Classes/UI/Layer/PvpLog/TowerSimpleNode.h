//
//  TowerSimpleNode.h
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef TowerSimpleNode_h
#define TowerSimpleNode_h

#include "cocos2d.h"

USING_NS_CC;

class TowerSimpleNode : public Node
{
public:
    static TowerSimpleNode* create();
    virtual ~TowerSimpleNode();
    
    void setWin(bool win);
    void setPercentage(int p);
    
private:
    TowerSimpleNode();
    virtual bool init() override;
    
private:
    Label* _percentage;
};

#endif /* TowerSimpleNode_h */
