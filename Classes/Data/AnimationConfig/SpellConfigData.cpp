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

static const string suffix(".csb");

SpellConfigData::SpellConfigData(tinyxml2::XMLElement *xmlElement)
:_isCasterEffectOnBody(false)
,_isReceiverEffectOnBody(false)
,_casterVolumeRate(1.0f)
,_receiverVolumeRate(1.0f)
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
            const char *data = xmlElement->Attribute("caster_is_body");
            if (data && strlen(data) > 0) {
                _isCasterEffectOnBody = (atoi(data) != 0) ? true : false;
            }
        }
        {
            const char *data = xmlElement->Attribute("caster_volume_rate");
            if (data && strlen(data) > 0) {
                _casterVolumeRate = atof(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("resource_name");
            if (data && strlen(data) > 0) {
                vector<string> v;
                Utils::split(v, data, ",");
                for (int i = 0; i < v.size(); ++i) {
                    const string& name = v.at(i);
                    const size_t found = name.find_last_of(".");
                    if (found != string::npos && name.substr(found + 1) == "plist") {
                        _resourceNames.push_back("particle/" + name);
                    } else if (name.length() > 0) {
                        _resourceNames.push_back(name + suffix);
                    }
                }
            }
        }
        {
            const char *data = xmlElement->Attribute("receiver_is_body");
            if (data && strlen(data) > 0) {
                _isReceiverEffectOnBody = (atoi(data) != 0) ? true : false;
            }
        }
        {
            const char *data = xmlElement->Attribute("receiver_volume_rate");
            if (data && strlen(data) > 0) {
                _receiverVolumeRate = atof(data);
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

bool SpellConfigData::isCasterEffectOnBody() const
{
    return _isCasterEffectOnBody;
}

float SpellConfigData::getCasterVolumeRate() const
{
    return _casterVolumeRate;
}

const vector<string>& SpellConfigData::getReceiverResourceNames() const
{
    return _resourceNames;
}

bool SpellConfigData::isReceiverEffectOnBody() const
{
    return _isReceiverEffectOnBody;
}

float SpellConfigData::getReceiverVolumeRate() const
{
    return _receiverVolumeRate;
}
