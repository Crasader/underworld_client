//
//  TalentUpgradeData.h
//  Underworld_Client
//
//  Created by Andy on 16/6/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef TalentUpgradeData_h
#define TalentUpgradeData_h

#include <iostream>

namespace tinyxml2 { class XMLElement; }

class TalentUpgradeData
{
public:
    TalentUpgradeData(tinyxml2::XMLElement *xmlElement);
    virtual ~TalentUpgradeData();
    
    int getId() const;
    int getLevel() const;
    int getGold() const;
    int getBook() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    
private:
    int _id;
    int _level;
    int _gold;
    int _book;
    std::string _name;
    std::string _description;
};

#endif /* TalentUpgradeData_h */
