//
//  TalentInfoNode.h
//  Underworld_Client
//
//  Created by Andy on 16/6/29.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef TalentInfoNode_h
#define TalentInfoNode_h

#include "cocos2d.h"

USING_NS_CC;

class TalentBriefNode;

class TalentInfoNode : public Node
{
public:
    typedef std::function<void(const std::string&, int)> Callback;
    
public:
    static TalentInfoNode* create(const Callback& callback);
    virtual ~TalentInfoNode();
    
    void update(const std::string& name);
    
protected:
    TalentInfoNode();
    bool init(const Callback& callback);
    void updateCost(int count);
    
private:
    std::string _name;
    int _cost;
    Callback _callback;
    std::vector<TalentBriefNode*> _nodes;
    Label* _costLabel;
};

#endif /* TalentInfoNode_h */
