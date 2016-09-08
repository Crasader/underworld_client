//
//  GuildTorphyNode.h
//  Underworld_Client
//
//  Created by Andy on 16/9/8.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef GuildTorphyNode_h
#define GuildTorphyNode_h

#include "cocos2d.h"

USING_NS_CC;

class GuildTorphyNode : public Node
{
public:
    static GuildTorphyNode* create();
    virtual ~GuildTorphyNode();
    
    void setCount(int count);
    
private:
    GuildTorphyNode();
    virtual bool init() override;
    
private:
    Label* _trophy;
};

#endif /* GuildTorphyNode_h */
