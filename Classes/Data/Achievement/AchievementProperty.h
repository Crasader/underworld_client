//
//  AchievementProperty.h
//  Underworld_Client
//
//  Created by Andy on 15/12/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef AchievementProperty_h
#define AchievementProperty_h

#include "QuestProperty.h"

class AchievementProperty : public QuestProperty
{
public:
    AchievementProperty(tinyxml2::XMLElement *xmlElement);
    virtual ~AchievementProperty();
};

#endif /* AchievementProperty_h */
