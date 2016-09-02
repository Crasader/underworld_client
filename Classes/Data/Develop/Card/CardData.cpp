//
//  CardData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardData.h"
#include "cocostudio/CocoStudio.h"
#include "SkillData.h"
#include "RuneData.h"
#include "CardProperty.h"

using namespace std;
using namespace cocostudio;

CardData::CardData(const rapidjson::Value& jsonDict)
:CardSimpleData(jsonDict)
{
    update(jsonDict);
}

CardData::CardData(const CardSimpleData& instance)
:CardSimpleData(instance) {}

CardData::~CardData()
{
    for (auto data : _skills) {
        CC_SAFE_DELETE(data);
    }
    
    for (auto iter = begin(_runes); iter != end(_runes); ++iter) {
        CC_SAFE_DELETE(iter->second);
    }
}

void CardData::update(const rapidjson::Value& jsonDict)
{
    CardSimpleData::update(jsonDict);
    {
        static const char* key("skills");
        if (DICTOOL->checkObjectExist_json(jsonDict, key)) {
            auto property(dynamic_cast<const CardProperty*>(getProperty()));
            if (property) {
                const auto& skills(property->getSkills());
                for (int i = 0; i < DICTOOL->getArrayCount_json(jsonDict, key); ++i) {
                    if (i < skills.size()) {
                        auto value = DICTOOL->getIntValueFromArray_json(jsonDict, key, i);
                        _skills.push_back(new (nothrow) SkillData(skills.at(i), value));
                    }
                }
            }
        }
    }
    
    {
        static const char* key("runes");
        if (DICTOOL->checkObjectExist_json(jsonDict, key)) {
            for (int i = 0; i < DICTOOL->getArrayCount_json(jsonDict, key); ++i) {
                const auto& value = DICTOOL->getDictionaryFromArray_json(jsonDict, key, i);
                auto data = new (nothrow) RuneData(value);
                if (_runes.find(i) == end(_runes)) {
                    _runes.insert(make_pair(i, data));
                }
            }
        }
    }
}

void CardData::removeRune(int idx)
{
    if (_runes.find(idx) != end(_runes)) {
        _runes.erase(idx);
    }
}

void CardData::updateRune(int idx, const rapidjson::Value& jsonDict)
{
    if (_runes.find(idx) != end(_runes)) {
        _runes.at(idx)->update(jsonDict);
    } else {
        _runes.insert(make_pair(idx, new (nothrow) RuneData(jsonDict)));
    }
}

const vector<SkillData*>& CardData::getSkills() const
{
    return _skills;
}

const RuneData* CardData::getRune(int idx) const
{
    if (_runes.find(idx) != end(_runes)) {
        return _runes.at(idx);
    }
    
    return nullptr;
}
