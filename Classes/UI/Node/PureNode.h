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
    enum class Type {
        White,
        Gray,
        Black
    };
    
    static PureNode* create(Type type, const Size& size);
    virtual ~PureNode();
    
    void setType(Type type);
    void setSize(const Size& size);
    
private:
    PureNode();
    bool init(Type type, const Size& size);
    std::string getResourceFile(Type type) const;
    
private:
    Type _type;
    Sprite* _sprite;
};

#endif /* PureNode_h */
