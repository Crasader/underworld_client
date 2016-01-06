//
//  HeroPieceData.h
//  Underworld_Client
//
//  Created by Andy on 16/1/6.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef HeroPieceData_h
#define HeroPieceData_h

#include <iostream>
#include <vector>

namespace tinyxml2 { class XMLElement; }

class RewardData;

class HeroPieceData
{
public:
    HeroPieceData(tinyxml2::XMLElement *xmlElement);
    virtual ~HeroPieceData();
    
    int getId() const;
    int getRequiredCount() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    const std::vector<RewardData*>& getSoldRewards() const;
    
private:
    int _id;
    int _requiredCount;
    std::string _name;
    std::string _description;
    std::vector<RewardData*> _soldRewards;
};

#endif /* HeroPieceData_h */
