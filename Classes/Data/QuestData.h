//
//  QuestData.h
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

// =====================================================
// Quest Reward
// =====================================================

class QuestReward
{
public:
    QuestReward(const std::string& content);
    virtual ~QuestReward();
    
    int getId() const;
    int getCount() const;
    
private:
    int _id;
    int _count;
};

// =====================================================
// Quest Data
// =====================================================

class QuestData
{
public:
    QuestData(tinyxml2::XMLElement *xmlElement);
    virtual ~QuestData();
    
    int getId() const;
    int getRequiredCount() const;
    const std::vector<QuestReward *>& getRewards() const;
    const std::string& getDescription() const;
    
private:
    int _id;
    int _requiredCount;
    std::vector<QuestReward *> _rewards;
    std::string _description;
};

#endif /* QuestData_h */
