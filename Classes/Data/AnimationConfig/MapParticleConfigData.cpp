//
//  MapParticleConfigData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/18.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MapParticleConfigData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"

using namespace std;

MapParticleConfigData::MapParticleConfigData(tinyxml2::XMLElement *xmlElement)
:_posX(0.0f)
,_posY(0.0f)
,_scaleX(1.0f)
,_scaleY(1.0f)
{
    if (xmlElement) {
        {
            string data = xmlElement->Attribute("name");
            if (data.find(".csb") != string::npos) {
                _name = data;
            } else {
                static const string prefix("particle/");
                static const string suffix(".plist");
                if (data.length() > 0) {
                    _name = prefix + data + suffix;
                }
            }
        }
        {
            const char *data = xmlElement->Attribute("pos_x");
            if (data && strlen(data) > 0) {
                _posX = Utils::stof(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("pos_y");
            if (data && strlen(data) > 0) {
                _posY = Utils::stof(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("scale_x");
            if (data && strlen(data) > 0) {
                _scaleX = Utils::stof(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("scale_y");
            if (data && strlen(data) > 0) {
                _scaleY = Utils::stof(data);
            }
        }
    }
}

MapParticleConfigData::~MapParticleConfigData()
{
    
}

const std::string& MapParticleConfigData::getName() const
{
    return _name;
}

float MapParticleConfigData::getPosX() const
{
    return _posX;
}

float MapParticleConfigData::getPosY() const
{
    return _posY;
}

float MapParticleConfigData::getScaleX() const
{
    return _scaleX;
}

float MapParticleConfigData::getScaleY() const
{
    return _scaleY;
}