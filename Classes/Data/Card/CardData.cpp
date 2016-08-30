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

using namespace std;
using namespace cocostudio;

CardData::CardData(const rapidjson::Value& jsonDict)
:CardSimpleData(jsonDict)
{
    {
        static const char* key("skills");
        if (DICTOOL->checkObjectExist_json(jsonDict, key)) {
            for (int i = 0; i < DICTOOL->getArrayCount_json(jsonDict, key); ++i) {
                auto value = DICTOOL->getIntValueFromArray_json(jsonDict, key, i);
                _skills.push_back(new (nothrow) SkillData(value));
            }
        }
    }
    
    {
        static const char* key("runes");
        if (DICTOOL->checkObjectExist_json(jsonDict, key)) {
            for (int i = 0; i < DICTOOL->getArrayCount_json(jsonDict, key); ++i) {
                const auto& value = DICTOOL->getDictionaryFromArray_json(jsonDict, key, i);
                _runes.push_back(new (nothrow) RuneData(value));
            }
        }
    }
}

CardData::~CardData() {}

const vector<SkillData*>& CardData::getSkills() const
{
    return _skills;
}

const vector<RuneData*>& CardData::getRunes() const
{
    return _runes;
}
