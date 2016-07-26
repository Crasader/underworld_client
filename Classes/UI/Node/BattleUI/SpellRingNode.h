//
//  SpellRingNode.h
//  Underworld_Client
//
//  Created by Andy on 16/7/25.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef SpellRingNode_h
#define SpellRingNode_h

#include "cocos2d.h"

USING_NS_CC;

class SpellRingNode : public Node
{
public:
    typedef std::function<void(Node*)> Callback;
    
    static SpellRingNode* create(const std::string& file, float duration, const Callback& callback);
    virtual ~SpellRingNode();
    
private:
    SpellRingNode();
    bool init(const std::string& file, float duration, const Callback& callback);
    void onExpire(float dt);
    
private:
    Callback _callback;
};

#endif /* SpellRingNode_h */
