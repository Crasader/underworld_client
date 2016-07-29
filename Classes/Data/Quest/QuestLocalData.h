//
//  QuestLocalData.h
//  Underworld_Client
//
//  Created by Andy on 15/11/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef QuestLocalData_h
#define QuestLocalData_h

#include "AbstractLocalData.h"
#include <vector>

class ConditionData;
class ContentData;
class ObjectBriefData;

class QuestLocalData : public AbstractLocalData
{
public:
    QuestLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~QuestLocalData();
    
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

#endif /* QuestLocalData_h */
