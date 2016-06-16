//
//  FormationUnitNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/6/14.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "FormationUnitNode.h"
#include "CocosUtils.h"

using namespace std;

FormationUnitNode* FormationUnitNode::create(const string& name)
{
    FormationUnitNode *ret = new (nothrow) FormationUnitNode();
    if (ret && ret->init(name))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

FormationUnitNode::FormationUnitNode() {}

FormationUnitNode::~FormationUnitNode()
{
    removeAllChildren();
}

bool FormationUnitNode::init(const string& name)
{
    if (Widget::init())
    {
        _name = name;
        
        auto sprite = CocosUtils::playAnimation("fatso-stand/fatso-stand-3", 10, true, 0, DEFAULT_FRAME_DELAY, nullptr);
        addChild(sprite);
        
        return true;
    }
    
    return false;
}

const string& FormationUnitNode::getUnitName() const
{
    return _name;
}
