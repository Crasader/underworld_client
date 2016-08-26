//
//  CardPropertyNode.h
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardPropertyNode_h
#define CardPropertyNode_h

#include "cocos2d.h"

USING_NS_CC;

class CardPropertyNode : public Node {
public:
    static CardPropertyNode* create();
    virtual ~CardPropertyNode();
    void setProperty();
    
private:
    CardPropertyNode();
    virtual bool init() override;
    
private:
    Sprite* _icon;
    Label* _name;
    Label* _data;
};

#endif /* CardPropertyNode_h */
