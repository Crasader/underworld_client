//
//  BattleBattleResourceNode.h
//  Underworld_Client
//
//  Created by Andy on 16/2/18.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef BattleBattleResourceNode_h
#define BattleBattleResourceNode_h

#include "cocos2d.h"
#include "CocosGlobal.h"

USING_NS_CC;

class BattleResourceNode : public Node
{
public:
    static BattleResourceNode* create(ResourceType type);
    virtual ~BattleResourceNode();
    
    ResourceType getType() const;
    int getCount() const;
    float getSubCount() const;
    
    void setType(ResourceType type);
    void setCount(int count, bool animated);
    void setSubCount(float count);
    
protected:
    BattleResourceNode();
    bool init(ResourceType type);
    
private:
    ResourceType _type;
    int _count;
    float _subCount;
    
    Sprite* _icon;
    Label* _countLabel;
    Label* _subCountLabel;
    ProgressTimer* _progressTimer;
};

#endif /* BattleBattleResourceNode_h */
