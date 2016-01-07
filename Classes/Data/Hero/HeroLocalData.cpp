//
//  HeroLocalData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "HeroLocalData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "AttributeData.h"
#include "DataManager.h"
#include "SkillLocalData.h"
#include "ArtifactLocalData.h"
#include "DataManager.h"

using namespace std;

HeroLocalData::HeroLocalData(tinyxml2::XMLElement *xmlElement)
:AbstractLocalData(xmlElement)
,_skillId(0)
{
    if (xmlElement) {
        {
            const char *data = xmlElement->Attribute("attr");
            if (data) {
                vector<string> result;
                Utils::split(result, data, ",", "");
                for (vector<string>::const_iterator iter = result.begin(); iter != result.end(); ++iter)
                {
                    AttributeData* attr = new (nothrow) AttributeData(*iter);
                    _attributes.insert(make_pair(attr->getId(), attr));
                }
            }
        }
        {
            const char *data = xmlElement->Attribute("skill");
            if (data) {
                _skillId = atoi(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("equipment");
            if (data) {
                vector<string> result;
                Utils::split(result, data, ",", "");
                for (vector<string>::const_iterator iter = result.begin(); iter != result.end(); ++iter)
                {
                    _artifactIds.push_back(atoi((*iter).c_str()));
                }
            }
        }
    }
}

HeroLocalData::~HeroLocalData()
{
    
}

const map<int, AttributeData *>& HeroLocalData::getAttributes() const
{
    return _attributes;
}

const AttributeData* HeroLocalData::getAttribute(int id) const
{
    if (_attributes.find(id) != _attributes.end()) {
        return _attributes.at(id);
    }
    
    return nullptr;
}

const SkillLocalData* HeroLocalData::getSkillData() const
{
    return DataManager::getInstance()->getSkillData(_skillId);
}

const vector<int>& HeroLocalData::getArtifacts() const
{
    return _artifactIds;
}

const ArtifactLocalData* HeroLocalData::getUnlockedArtifactData(int index) const
{
    if (index > _artifactIds.size()) {
        return DataManager::getInstance()->getArtifactData(_artifactIds.at(index));
    }
    
    return nullptr;
}
