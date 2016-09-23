//
//  CardData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardData.h"
#include "JSonUtils.h"
#include "SkillData.h"
#include "RuneData.h"
#include "CardProperty.h"

using namespace std;

CardData::CardData(const rapidjson::Value& jsonDict)
:AbstractData(jsonDict)
{
    update(jsonDict);
}

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
    AbstractData::update(jsonDict);
    do {
        static const char* key("skills");
        CC_BREAK_IF(!JSonUtils::isExist(jsonDict, key));
        auto property(dynamic_cast<const CardProperty*>(getProperty()));
        CC_BREAK_IF(!property);
        const auto& skills(property->getSkills());
        for (int i = 0; i < DICTOOL->getArrayCount_json(jsonDict, key); ++i) {
            CC_BREAK_IF(i >= skills.size());
            auto value = DICTOOL->getIntValueFromArray_json(jsonDict, key, i);
            if (_skills.size() > i) {
                _skills.at(i)->update(value);
            } else {
                _skills.push_back(new (nothrow) SkillData(skills.at(i), value));
            }
        }
    } while (false);
    
    do {
        static const char* key("runes");
        CC_BREAK_IF(!JSonUtils::isExist(jsonDict, key));
        for (int i = 0; i < DICTOOL->getArrayCount_json(jsonDict, key); ++i) {
            const auto& value = DICTOOL->getDictionaryFromArray_json(jsonDict, key, i);
            updateRune(i, value);
        }
    } while (false);
}

void CardData::removeRune(int idx)
{
    auto iter(_runes.find(idx));
    if (iter != end(_runes)) {
        _runes.erase(iter);
    }
}

void CardData::updateRune(int idx, const rapidjson::Value& jsonDict)
{
    auto iter(_runes.find(idx));
    if (iter != end(_runes)) {
        iter->second->update(jsonDict);
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
    auto iter(_runes.find(idx));
    if (_runes.find(idx) != end(_runes)) {
        return iter->second;
    }
    
    return nullptr;
}
