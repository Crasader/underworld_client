//
//  CardLocalData.h
//  Underworld_Client
//
//  Created by Andy on 16/6/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardLocalData_h
#define CardLocalData_h

#include <iostream>

namespace tinyxml2 { class XMLElement; }

class CardLocalData
{
public:
    CardLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~CardLocalData();
    
    int getId() const;
    int getType() const;
    int getStatus() const;
    int getFood() const;
    int getQuality() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    const std::string& getUnlockInfo() const;
    
private:
    int _id;
    int _type;
    int _status;
    int _food;
    int _quality;
    std::string _name;
    std::string _description;
    std::string _unlockInfo;
};

#endif /* CardLocalData_h */
