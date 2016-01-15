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

const vector<string>& SpellConfigData::getResourceNames() const
{
    return _resourceNames;
}
