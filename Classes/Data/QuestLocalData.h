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

class RewardData;

class QuestLocalData
{
public:
    QuestLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~QuestLocalData();
    
    int getId() const;
    int getRequiredCount() const;
    const std::vector<RewardData *>& getRewards() const;
    const std::string& getDescription() const;
    
private:
    int _id;
    int _requiredCount;
    std::vector<RewardData *> _rewards;
    std::string _description;
};

#endif /* QuestData_h */
