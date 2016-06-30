//
//  LevelLocalData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/12.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef LevelLocalData_h
#define LevelLocalData_h

#include <iostream>
#include <vector>

namespace tinyxml2 { class XMLElement; }

class LevelLocalData
{
public:
    LevelLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~LevelLocalData();
    
    int getLevel() const;
    int getExp() const;
    int getHp() const;
    int getDefence() const;
    const std::vector<int>& unlockedStages() const;
    const std::vector<int>& unlockedIcons() const;
    const std::vector<int>& unlockedQuests() const;
    
private:
    int _level;
    int _exp;
    int _hp;
    int _defence;
    std::vector<int> _stages;
    std::vector<int> _icons;
    std::vector<int> _quests;
};

#endif /* LevelLocalData_h */
