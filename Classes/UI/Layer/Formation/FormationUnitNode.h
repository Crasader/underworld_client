//
//  FormationUnitNode.h
//  Underworld_Client
//
//  Created by Andy on 16/6/14.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef FormationUnitNode_h
#define FormationUnitNode_h

#include "ui/CocosGUI.h"

using namespace cocos2d::ui;

class FormationUnitNode : public Widget
{
public:
    static FormationUnitNode* create(const std::string& name);
    virtual ~FormationUnitNode();
    
    const std::string& getUnitName() const;
    
protected:
    FormationUnitNode();
    bool init(const std::string& name);
    
protected:
    std::string _name;
};

#endif /* FormationUnitNode_h */
