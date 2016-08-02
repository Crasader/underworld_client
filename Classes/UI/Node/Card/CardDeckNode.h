//
//  CardDeckNode.h
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardDeckNode_h
#define CardDeckNode_h

#include "cocos2d.h"

USING_NS_CC;

class CardSimpleData;
class CardSimpleNode;

class CardDeckNode : public Node
{
public:
    static CardDeckNode* create();
    virtual ~CardDeckNode();
    void update(const std::vector<CardSimpleData*>& vec);
    
private:
    CardDeckNode();
    virtual bool init() override;
    
private:
    std::vector<CardSimpleNode*> _nodes;
};

#endif /* CardDeckNode_h */
