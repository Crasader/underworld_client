//
//  SpellConfigData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/12.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "SpellConfigData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"

using namespace std;
using namespace UnderWorld::Core;

static const string suffix(".csb");

SpellConfigData::SpellConfigData(tinyxml2::XMLElement *xmlElement)
:_casterSpellPosition(kBody)
,_isCasterShakeScreen(false)
,_receiverSpellPosition(kBody)
{
    if (xmlElement) {
        {
            const char *data = xmlElement->Attribute("spell_name");
            if (data && strlen(data) > 0) {
                _spellName = data;
            }
        }
        {
            const char *data = xmlElement->Attribute("caster_resource_name");
            if (data && strlen(data) > 0) {
                vector<string> v;
                Utils::split(v, data, ",");
                for (int i = 0; i < v.size(); ++i) {
                    const string& name = v.at(i);
                    if (name.length() > 0) {
                        _casterResourceNames.push_back(name + suffix);
                    }
                }
            }
        }
        {
            const char *data = xmlElement->Attribute("caster_spell_pos");
            if (data && strlen(data) > 0) {
                _casterSpellPosition = static_cast<SpellPosition>(atoi(data));
            }
        }
        {
            const char *data = xmlElement->Attribute("caster_shake_screen");
            if (data && strlen(data) > 0) {
                _isCasterShakeScreen = (atoi(data) != 0) ? true : false;
            }
        }
        {
            const char *data = xmlElement->Attribute("receiver_resource_name");
            if (data && strlen(data) > 0) {
                vector<string> v;
                Utils::split(v, data, ",");
                for (int i = 0; i < v.size(); ++i) {
                    const string& name = v.at(i);
                    const size_t found = name.find_last_of(".");
                    if (found != string::npos && name.substr(found + 1) == "plist") {
                        _receiverResourceNames.push_back("particle/" + name);
                    } else if (name.length() > 0) {
                        _receiverResourceNames.push_back(name + suffix);
                    }
                }
            }
        }
        {
            const char *data = xmlElement->Attribute("receiver_spell_pos");
            if (data && strlen(data) > 0) {
                _receiverSpellPosition = static_cast<SpellPosition>(atoi(data));
            }
        }
        {
            const char *data = xmlElement->Attribute("receiver_speed_rate");
            if (data && strlen(data) > 0) {
                vector<string> v;
                Utils::split(v, data, "_");
                for (int i = 0; i < v.size(); ++i) {
                    const SkillClass sc = static_cast<SkillClass>(i);
                    const float rate = atof(v.at(i).c_str());
                    _receiverSpeedRates.insert(make_pair(sc, rate));
                }
            }
        }
        {
            const char *data = xmlElement->Attribute("receiver_volume_rate");
            if (data && strlen(data) > 0) {
                vector<string> v;
                Utils::split(v, data, "_");
                for (int i = 0; i < v.size(); ++i) {
                    const SkillClass sc = static_cast<SkillClass>(i);
                    const float rate = atof(v.at(i).c_str());
                    _receiverVolumeRates.insert(make_pair(sc, rate));
                }
            }
        }
    }
}

SpellConfigData::~SpellConfigData()
{
    
}

const string& SpellConfigData::getSpellName() const
{
    return _spellName;
}

const vector<string>& SpellConfigData::getCasterResourceNames() const
{
    return _casterResourceNames;
}

SpellConfigData::SpellPosition SpellConfigData::getCasterSpellPosition() const
{
    return _casterSpellPosition;
}

bool SpellConfigData::isCasterShakeScreen() const
{
    return _isCasterShakeScreen;
}

const vector<string>& SpellConfigData::getReceiverResourceNames() const
{
    return _receiverResourceNames;
}

SpellConfigData::SpellPosition SpellConfigData::getReceiverSpellPosition() const
{
    return _receiverSpellPosition;
}

const map<SkillClass, float>& SpellConfigData::getReceiverSpeedRates() const
{
    return _receiverSpeedRates;
}

float SpellConfigData::getReceiverSpeedRate(SkillClass sc) const
{
    if (_receiverSpeedRates.find(sc) != _receiverSpeedRates.end()) {
        return _receiverSpeedRates.at(sc);
    }
    
    return 1.0f;
}

const map<SkillClass, float>& SpellConfigData::getReceiverVolumeRates() const
{
    return _receiverVolumeRates;
}

float SpellConfigData::getReceiverVolumeRate(SkillClass sc) const
{
    if (_receiverVolumeRates.find(sc) != _receiverVolumeRates.end()) {
        return _receiverVolumeRates.at(sc);
    }
    
    return 1.0f;
}
