//
//  ArtifactLocalData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ArtifactLocalData_h
#define ArtifactLocalData_h

#include "AbstractLocalData.h"
#include <unordered_map>

class AttributeData;
class SkillLocalData;

class ArtifactLocalData : public AbstractLocalData
{
public:
    ArtifactLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~ArtifactLocalData();
    
    const std::unordered_map<int, AttributeData *>& getAttributes() const;
    const AttributeData* getAttribute(int id) const;
    const SkillLocalData* getSkillData() const;
    
private:
    std::unordered_map<int, AttributeData *> _attributes;
    int _skillId;
};

#endif /* ArtifactLocalData_h */
