//
//  ObjectLocalData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/12.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ObjectLocalData_h
#define ObjectLocalData_h

#include <iostream>
#include <vector>

namespace tinyxml2 { class XMLElement; }

class RewardData;

class ObjectLocalData
{
public:
    ObjectLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~ObjectLocalData();
    
    int getObjectId() const;
    const std::vector<RewardData *>& getRewards() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    
private:
    int _objectId;
    std::vector<RewardData *> _rewards;
    std::string _name;
    std::string _description;
};

#endif /* ObjectLocalData_h */
