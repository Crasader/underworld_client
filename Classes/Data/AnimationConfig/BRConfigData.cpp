//
//  BRConfigData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/4/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "BRConfigData.h"
#include "tinyxml2/tinyxml2.h"
#include "CocosGlobal.h"
#include "Utils.h"
#include "CoreUtils.h"

using namespace std;

const std::string BRConfigData::TAIL_GAS_SPLITOR("|");

BRConfigData::BRConfigData(tinyxml2::XMLElement *xmlElement)
: _tailGasInterval(0)
{
    if (xmlElement) {
        _name = xmlElement->Attribute("render_key");
        {
            const char *data = xmlElement->Attribute("resource");
            if (data && strlen(data) > 0) {
                _resource.init(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("shadow_resource");
            if (data && strlen(data) > 0) {
                _shadowResource.init(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("explode_resource");
            if (data && strlen(data) > 0) {
                _explodeResource.init(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("explode_sound");
            if (data && strlen(data) > 0) {
                _explodeSound = data;
            }
        }
        {
            _material = BulletMaterial::Arrow;
            const char* data = xmlElement->Attribute("material");
            if (data && strlen(data) > 0) {
                if (data == std::string("arrow")) {
                    _material = BulletMaterial::Arrow;
                } else if (data == std::string("cannon")) {
                    _material = BulletMaterial::Cannon;
                }
            }
        }
        {
            _tailGas.clear();
            const char* data = xmlElement->Attribute("tail_gas");
            if (data && strlen(data) > 0) {
                std::vector<std::string> tailGasVec;
                UnderWorld::Core::UnderWorldCoreUtils::split(tailGasVec, data, TAIL_GAS_SPLITOR);
                
                for (int i = 0; i < tailGasVec.size(); ++i) {
                    _tailGas.push_back(EffectData());
                    _tailGas.back().init(tailGasVec[i]);
                }
            }
        }
        {
            const char* data = xmlElement->Attribute("tail_gas_interval");
            if (data && strlen(data) > 0) {
                _tailGasInterval = Utils::stoi(data);
            }
            
        }
    }
}

BRConfigData::~BRConfigData()
{
    
}

const string& BRConfigData::getName() const
{
    return _name;
}

const EffectData& BRConfigData::getResource() const
{
    return _resource;
}

const EffectData& BRConfigData::getShadowResource() const
{
    return _shadowResource;
}

const EffectData& BRConfigData::getExplodeResource() const
{
    return _explodeResource;
}

const string& BRConfigData::getExplodeSound() const
{
    return _explodeSound;
}

BulletMaterial BRConfigData::getBulletMeterial() const {
    return _material;
}

const std::vector<EffectData>& BRConfigData::getTailGasResource() const {
    return _tailGas;
}

int BRConfigData::getTailGasInterval() const {
    return _tailGasInterval;
}
