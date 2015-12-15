//
//  QuestLocalData.h
//  Underworld_Client
//
//  Created by Andy on 15/11/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef QuestData_h
#define QuestData_h

#include <iostream>
#include <vector>

namespace tinyxml2 { class XMLElement; }

class ConditionData;
class ContentData;
class RewardData;

class QuestLocalData
{
public:
    QuestLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~QuestLocalData();
    
    int getId() const;
    int getRace() const;
    const std::vector<ConditionData*>& getConditions() const;
    const std::vector<ContentData*>& getContents() const;
    const std::vector<RewardData *>& getRewards() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    
private:
    int _id;
    int _race;
    std::vector<ConditionData*> _conditions;
    std::vector<ContentData*> _contents;
    std::vector<RewardData*> _rewards;
    std::string _name;
    std::string _description;
};

#endif /* QuestData_h */
