//
//  HeroData.h
//  Underworld_Client
//
//  Created by Andy on 16/1/5.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef HeroData_h
#define HeroData_h

#include "AbstractData.h"
#include <unordered_map>

class AttributeData;
class HeroLocalData;
class HeroUpgradeData;
class ArtifactData;

class HeroData : public AbstractData
{
public:
    HeroData(const rapidjson::Value& jsonDict);
    virtual ~HeroData();
    
    const HeroLocalData* getLocalData() const;
    const HeroUpgradeData* getUpgradeData() const;
    const ArtifactData* getArtifactData(int index) const;
    
    // setters
    void setLevel(int level);
    void setArtifact(int index, const ArtifactData* artifact);
    void setArtifactLevel(int index, int level);
    
private:
    void calculateTotalAttributes();    
    
private:
    std::unordered_map<int, AttributeData*> _attributes;
    std::unordered_map<int, ArtifactData*> _artifacts;
};

#endif /* HeroData_h */
