//
//  TalentBriefNode.h
//  Underworld_Client
//
//  Created by Andy on 16/6/28.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef TalentBriefNode_h
#define TalentBriefNode_h

#include "cocos2d.h"

USING_NS_CC;

class TalentBriefNode : public Node
{
public:
    static TalentBriefNode* create();
    virtual ~TalentBriefNode();
    
    void update(const std::string& name, int level, const std::string& brief);
    
protected:
    TalentBriefNode();
    virtual bool init() override;
    
private:
    Label* _name;
    Label* _level;
    Label* _brief;
};

#endif /* TalentBriefNode_h */
