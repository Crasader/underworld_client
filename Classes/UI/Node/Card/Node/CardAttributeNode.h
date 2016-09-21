//
//  CardAttributeNode.h
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardAttributeNode_h
#define CardAttributeNode_h

#include "cocos2d.h"
#include "ObjectUtils.h"

USING_NS_CC;

class PureNode;

class CardAttributeNode : public Node {
public:
    static CardAttributeNode* create(const Color4B& color);
    virtual ~CardAttributeNode();
    void setColor(const Color4B& color);
    void setAttribute(ObjectUtils::CardAttributeType type, float value);
    void setName(const std::string& name);
    
private:
    CardAttributeNode();
    bool init(const Color4B& color);
    void getInfo(ObjectUtils::CardAttributeType type, std::string& icon, std::string& name) const;
    
private:
    PureNode* _background;
    Sprite* _icon;
    Label* _name;
    Label* _data;
};

#endif /* CardAttributeNode_h */
