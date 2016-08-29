//
//  PureNode.h
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef PureNode_h
#define PureNode_h

#include "cocos2d.h"

USING_NS_CC;

class PureNode: public Node
{
public:
    static PureNode* create(const Color4B& color, const Size& size);
    static PureNode* createLine(const Size& size);
    virtual ~PureNode();
    
    void setColor(const Color4B& color);
    void setSize(const Size& size);
    
private:
    PureNode();
    bool init(const Color4B& color, const Size& size);
    
private:
    Color4B _color;
    Sprite* _sprite;
};

#endif /* PureNode_h */
