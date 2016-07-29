//
//  ObjectBriefNode.h
//  Underworld_Client
//
//  Created by Andy on 16/7/29.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef ObjectBriefNode_h
#define ObjectBriefNode_h

#include "cocos2d.h"
#include "CocosGlobal.h"

USING_NS_CC;

class ObjectBriefData;

class ObjectBriefNode : public Node
{
public:
    static ObjectBriefNode* create(const ObjectBriefData* data);
    virtual ~ObjectBriefNode();
    
    void update(const ObjectBriefData* data);
    const ObjectBriefData* getData() const;
    
protected:
    ObjectBriefNode();
    bool init(const ObjectBriefData* data);
    
private:
    Sprite* _icon;
    Label* _count;
    const ObjectBriefData* _data;
};

#endif /* ObjectBriefNode_h */
