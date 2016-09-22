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

SpellConfigData::SpellConfigData(tinyxml2::XMLElement *xmlElement)
: _spellDirection(kNone)
, _spellPosition(kBody)
, _spellRenderLayer(0)
, _spellHeight(0)
, _loop(true)
{
    if (xmlElement) {
        {
            const char *data = xmlElement->Attribute("render_key");
            if (data && strlen(data) > 0) {
                _renderKey = data;
            }
        }
        {
            const char *data = xmlElement->Attribute("spell_direction");
            if (data && strlen(data) > 0) {
                _spellDirection = static_cast<Direction>(atoi(data));
            } else {
                _spellDirection = kNone;
            }
        }
        {
            const char *data = xmlElement->Attribute("spell_position");
            if (data && strlen(data) > 0) {
                _spellPosition = static_cast<Position>(atoi(data));
            } else {
                _spellPosition = kBody;
            }
        }
        {
            const char *data = xmlElement->Attribute("spell_render_layer");
            if (data && strlen(data) > 0) {
                _spellRenderLayer = atoi(data);
            } else {
                _spellRenderLayer = 0;
            }
        }
        {
            const char *data = xmlElement->Attribute("spell_height");
            if (data && strlen(data) > 0) {
                _spellHeight = atoi(data);
            } else {
                _spellHeight = 0;
            }
        }
        {
            const char *data = xmlElement->Attribute("loop");
            if (data && strlen(data) > 0) {
                _loop = (atoi(data) != 0);
            } else {
                _loop = true;
            }
        }
        {
            const char *data = xmlElement->Attribute("foreground_resource");
            if (data && strlen(data) > 0) {
                _foregroundResource.init(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("background_resource");
            if (data && strlen(data) > 0) {
                _backgroundResource.init(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("effect_sound");
            if (data && strlen(data) > 0) {
                _effectSound = data;
            }
        }
    }
}

SpellConfigData::~SpellConfigData() {}