//
//  ArtifactLocalData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ArtifactLocalData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "AttributeData.h"
#include "DataManager.h"
#include "SkillLocalData.h"

using namespace std;

ArtifactLocalData::ArtifactLocalData(tinyxml2::XMLElement *xmlElement)
:AbstractLocalData(xmlElement)
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
    }
}

ArtifactLocalData::~ArtifactLocalData()
{
    Utils::clearMap(_attributes);
}

const map<int, AttributeData *>& ArtifactLocalData::getAttributes() const
{
    return _attributes;
}

const AttributeData* ArtifactLocalData::getAttribute(int id) const
{
    if (_attributes.find(id) != _attributes.end()) {
        return _attributes.at(id);
    }
    
    return nullptr;
}

const SkillLocalData* ArtifactLocalData::getSkillData() const
{
    return DataManager::getInstance()->getSkillData(_skillId);
}
