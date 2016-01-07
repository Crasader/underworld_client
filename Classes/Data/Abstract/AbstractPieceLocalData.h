//
//  AbstractPieceLocalData.h
//  Underworld_Client
//
//  Created by Andy on 16/1/7.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef AbstractPieceLocalData_h
#define AbstractPieceLocalData_h

#include "AbstractLocalData.h"
#include <vector>

class RewardData;

class AbstractPieceLocalData : public AbstractLocalData
{
public:
    AbstractPieceLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~AbstractPieceLocalData() = 0;
    
    int getRequiredCount() const;
    const std::vector<RewardData*>& getSoldRewards() const;
    
private:
    int _requiredCount;
    std::vector<RewardData*> _soldRewards;
};

#endif /* AbstractPieceLocalData_h */
