//
//  AchievementLocalData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef AchievementLocalData_h
#define AchievementLocalData_h

#include "QuestLocalData.h"

class AchievementLocalData : public QuestLocalData
{
public:
    AchievementLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~AchievementLocalData();
};

#endif /* AchievementLocalData_h */
