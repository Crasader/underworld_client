//
//  LevelProperty.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/12.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "LevelProperty.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"

using namespace std;

LevelProperty::LevelProperty(tinyxml2::XMLElement *xmlElement)
:_level(0)
,_exp(0)
,_hp(0)
,_defence(0)
{
    if (xmlElement) {
        _level = atoi(xmlElement->Attribute("level"));
        _exp = atoi(xmlElement->Attribute("experience"));
        
        {
            const char *data = xmlElement->Attribute("hp");
            if (data) {
                _hp = atoi(data);
            }
        }
        
        {
            const char *data = xmlElement->Attribute("defence");
            if (data) {
                _defence = atoi(data);
            }
        }
        
        {
            const char *data = xmlElement->Attribute("stage");
            if (data) {
                vector<string> result;
                Utils::split(result, data, ",", "");
                for (int i = 0; i < result.size(); ++i) {
                    _stages.push_back(atoi(result.at(i).c_str()));
                }
            }
        }
        
        {
            const char *data = xmlElement->Attribute("icon");
            if (data) {
                vector<string> result;
                Utils::split(result, data, ",", "");
                for (int i = 0; i < result.size(); ++i) {
                    _icons.push_back(atoi(result.at(i).c_str()));
                }
            }
        }
        
        {
            const char *data = xmlElement->Attribute("quest");
            if (data) {
                vector<string> result;
                Utils::split(result, data, ",", "");
                for (int i = 0; i < result.size(); ++i) {
                    _quests.push_back(atoi(result.at(i).c_str()));
                }
            }
        }
    }
}

LevelProperty::~LevelProperty() {}

int LevelProperty::getLevel() const
{
    return _level;
}

int LevelProperty::getExp() const
{
    return _exp;
}

int LevelProperty::getHp() const
{
    return _hp;
}

int LevelProperty::getDefence() const
{
    return _defence;
}

const vector<int>& LevelProperty::unlockedStages() const
{
    return _stages;
}

const vector<int>& LevelProperty::unlockedIcons() const
{
    return _icons;
}

const vector<int>& LevelProperty::unlockedQuests() const
{
    return _quests;
}
