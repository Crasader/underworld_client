//
//  ResourceNode.h
//  Underworld_Client
//
//  Created by Andy on 15/12/15.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ResourceNode_h
#define ResourceNode_h

#include "cocos2d.h"
#include "CocosGlobal.h"

USING_NS_CC;

class ResourceNode : public Node
{
public:
    typedef std::function<void(ResourceNode*)> Callback;
    
    static ResourceNode* create(::ResourceType type, int count, const Callback& callback);
    virtual ~ResourceNode();
    ::ResourceType getType() const;
    int getCount() const;
    void setCount(int count, bool animated);
    
protected:
    ResourceNode();
    bool init(::ResourceType type, int count, const Callback& callback);
    
private:
    ::ResourceType _type;
    int _count;
    Callback _callback;
    Label *_countLabel;
};

#endif /* ResourceNode_h */
