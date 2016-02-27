//
//  BattleSmallResourceNode.h
//  Underworld_Client
//
//  Created by Andy on 16/2/27.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef BattleSmallResourceNode_h
#define BattleSmallResourceNode_h

#include "cocos2d.h"
#include "CocosGlobal.h"

USING_NS_CC;

class BattleSmallResourceNode : public Node
{
public:
    static BattleSmallResourceNode* create(ResourceType type, int count);
    virtual ~BattleSmallResourceNode();
    
    void check(float count);
    void setCount(int count);
    
    // getters
    ResourceType getType() const;
    int getCount() const;
    
protected:
    BattleSmallResourceNode();
    bool init(ResourceType type, int count);
    
private:
    ResourceType _type;
    int _count;
    
    Node* _icon;
    Label* _countLabel;
};

#endif /* BattleSmallResourceNode_h */
