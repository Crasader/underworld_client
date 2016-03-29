//
//  ArtifactData.h
//  Underworld_Client
//
//  Created by Andy on 16/1/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ArtifactData_h
#define ArtifactData_h

#include "AbstractData.h"
#include <unordered_map>
#include "CocosGlobal.h"

class AttributeData;
class ArtifactLocalData;
class ArtifactUpgradeData;

class ArtifactData : public AbstractData
{
public:
    ArtifactData(const rapidjson::Value& jsonDict);
    ArtifactData(const ArtifactData& instance);
    virtual ~ArtifactData();
    
    const std::unordered_map<int, AttributeData *>& getAttributes() const;
    const AttributeData* getAttribute(int id) const;
    
    const ArtifactLocalData* getLocalData() const;
    const ArtifactUpgradeData* getUpgradeData() const;
    
private:
    std::unordered_map<int, AttributeData *> _attributes;
};

#endif /* ArtifactData_h */
