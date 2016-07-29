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

class ObjectBriefData;

class AbstractPieceLocalData : public AbstractLocalData
{
public:
    AbstractPieceLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~AbstractPieceLocalData() = 0;
    
    int getRequiredCount() const;
    const std::vector<ObjectBriefData*>& getSoldRewards() const;
    
private:
    int _requiredCount;
    std::vector<ObjectBriefData*> _soldRewards;
};

#endif /* AbstractPieceLocalData_h */
