//
//  QuestProperty.h
//  Underworld_Client
//
//  Created by Andy on 15/11/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef QuestProperty_h
#define QuestProperty_h

#include "AbstractProperty.h"
#include <vector>

class ConditionData;
class ContentData;
class ObjectBriefData;

class QuestProperty : public AbstractProperty
{
public:
    QuestProperty(tinyxml2::XMLElement *xmlElement);
    virtual ~QuestProperty();
    
    int getRace() const;
    const std::vector<ConditionData*>& getConditions() const;
    const std::vector<ContentData*>& getContents() const;
    const std::vector<ObjectBriefData *>& getRewards() const;
    
private:
    int _race;
    std::vector<ConditionData*> _conditions;
    std::vector<ContentData*> _contents;
    std::vector<ObjectBriefData*> _rewards;
};

#endif /* QuestProperty_h */
