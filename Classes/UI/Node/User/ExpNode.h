//
//  ExpNode.h
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef ExpNode_h
#define ExpNode_h

#include "cocos2d.h"

USING_NS_CC;

class ExpNode : public Node
{
public:
    static ExpNode* create();
    virtual ~ExpNode();
    
    void setLevel(int level);
    void setPercentage(float p);
    
private:
    ExpNode();
    virtual bool init() override;
    
private:
    Label* _level;
    ProgressTimer* _expProgress;
};

#endif /* ExpNode_h */
