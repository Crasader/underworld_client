//
//  CardSimpleData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef CardSimpleData_h
#define CardSimpleData_h

#include <string>
#include "json/document.h"

namespace UnderWorld {
    namespace Core {
        class HMMCardType;
    }
}

class CardSimpleData
{
public:
    CardSimpleData(const rapidjson::Value& jsonDict);
    virtual ~CardSimpleData();
    
    bool operator==(const CardSimpleData& instance) const;
    
    int getIdx() const;
    int getLevel() const;
    int getCost() const;
    int getQuality() const;
    bool isHero() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    const std::string& getUnlockInfo() const;
    
private:
    int _idx;
    int _level;
    const UnderWorld::Core::HMMCardType* _cardType;
};

#endif /* CardSimpleData_h */
