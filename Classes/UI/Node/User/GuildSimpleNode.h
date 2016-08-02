//
//  GuildSimpleNode.h
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef GuildSimpleNode_h
#define GuildSimpleNode_h

#include "cocos2d.h"

USING_NS_CC;

class GuildSimpleNode : public Node
{
public:
    static GuildSimpleNode* create();
    virtual ~GuildSimpleNode();
    
    void setIcon(int idx);
    void setGuildName(const std::string& name);
    
private:
    GuildSimpleNode();
    virtual bool init() override;
    
private:
    Sprite* _icon;
    Label* _name;
};

#endif /* GuildSimpleNode_h */
