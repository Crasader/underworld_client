//
//  CardSimpleData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef CardSimpleData_h
#define CardSimpleData_h

#include "AbstractData.h"
#include <string>
#include "GameModeHMM.h"

namespace UnderWorld {
    namespace Core {
        class HMMCardType;
    }
}

class CardSimpleData : public AbstractData
{
public:
    CardSimpleData(const rapidjson::Value& jsonDict);
    virtual ~CardSimpleData();
    
    bool operator==(const CardSimpleData& instance) const;
    
    void update(const rapidjson::Value& jsonDict);
    UnderWorld::Core::HMMCardClass getCardClass() const;
    bool isHero() const;
    int getCost() const;
    int getQuality() const;
    virtual const std::string& getName() const override;
    virtual const std::string& getDescription() const override;
    const std::string& getUnlockInfo() const;
    
    virtual const AbstractProperty* getProperty() const override;
    
private:
    const UnderWorld::Core::HMMCardType* _cardType;
};

#endif /* CardSimpleData_h */
