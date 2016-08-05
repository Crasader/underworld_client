//
//  URConfigData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "CocosGlobal.h"
#include "URConfigData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"

using namespace std;


URConfigData::URConfigData(tinyxml2::XMLElement *xmlElement)
:_isFaceRight(false)
, _isMultiDirection(true)
, _isTransformOnHealth(false)
, _bodyScale(1.f)
, _effectScale(1.f)
, _bodyEffectPosX(0.f)
, _bodyEffectPosY(0.f)
, _headEffectPosX(0.f)
, _headEffectPoxY(0.f)
, _hpBarPosX(0.0f)
, _hpBarPosY(0.0f)
, _hpBarScaleX(1.0f)
, _moveDurationScale(1.f)
, _standDuraionScale(1.f)
{
    if (xmlElement) {
        _name = xmlElement->Attribute("render_key");
        {
            const char *data = xmlElement->Attribute("direction");
            if (data && strlen(data) > 0) {
                _isFaceRight = (atoi(data) != 0);
            } else {
                _isFaceRight = false;
            }
        }
        {
            const char *data = xmlElement->Attribute("multi_direction");
            if (data && strlen(data) > 0) {
                _isMultiDirection = (atoi(data) != 0);
            } else {
                _isMultiDirection = true;
            }
        }
        {
            const char *data = xmlElement->Attribute("transform_on_health");
            if (data && strlen(data) > 0) {
                _isTransformOnHealth = (atoi(data) != 0);
            } else {
                _isTransformOnHealth = false;
            }
        }
        {
            const char *data = xmlElement->Attribute("anim_type");
            if (data && strlen(data) > 0) {
                if (data == std::string("pvr")) {
                    _animationType = UnitAnimationType::PVR;
                } else if (data == std::string("csb")) {
                    _animationType = UnitAnimationType::CSB;
                } else {
                    _animationType = UnitAnimationType::UNIT_ANIMATION_TYPE_COUNT;
                }
            }
        }
        {
            const char *data = xmlElement->Attribute("normal_prefix");
            if (data && strlen(data) > 0) {
                _normalPrefix = data;
            }
        }
        {
            const char *data = xmlElement->Attribute("attck_sound");
            if (data && strlen(data) > 0) {
                _attckSound = std::string(AUDIO_PREFIX) + data + AUDIO_SUFFIX;
            }
        }
        {
            const char *data = xmlElement->Attribute("hurt_sound");
            if (data && strlen(data) > 0) {
                _hurtSound = std::string(AUDIO_PREFIX) + data + AUDIO_SUFFIX;
            }
        }
        {
            const char *data = xmlElement->Attribute("die_sound");
            if (data && strlen(data) > 0) {
                _dieSound = std::string(AUDIO_PREFIX) + data + AUDIO_SUFFIX;
            }
        }
        {
            const char *data = xmlElement->Attribute("body_scale");
            if (data) {
                _bodyScale = atof(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("effect_scale");
            if (data) {
                _effectScale = atof(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("body_effect_pos_x");
            if (data) {
                _bodyEffectPosX = atof(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("body_effect_pos_y");
            if (data) {
                _bodyEffectPosY = atof(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("head_effect_pos_x");
            if (data) {
                _headEffectPosX = atof(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("head_effect_pos_y");
            if (data) {
                _headEffectPoxY = atof(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("hp_bar_pos_x");
            if (data) {
                _hpBarPosX = atof(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("hp_bar_pos_y");
            if (data) {
                _hpBarPosY = atof(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("hp_bar_scale_x");
            if (data) {
                _hpBarScaleX = atof(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("move_duration_scale");
            if (data) {
                _moveDurationScale = atof(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("stand_duration_scale");
            if (data) {
                _standDuraionScale = atof(data);
            }
        }

    }
}

URConfigData::~URConfigData()
{
    
}

const string& URConfigData::getName() const
{
    return _name;
}
bool URConfigData::isFaceRight() const
{
    return _isFaceRight;
}

bool URConfigData::isMultiDirection() const
{
    return _isMultiDirection;
}
bool URConfigData::isTransformOnHealth() const
{
    return _isTransformOnHealth;
}

UnitAnimationType URConfigData::getAnimationType() const
{
    return _animationType;
}

const std::string& URConfigData::getNormalPrefix() const
{
    return _normalPrefix;
}

const string& URConfigData::getAttackSound() const
{
    return _attckSound;
}

const string& URConfigData::getHurtSound() const
{
    return _hurtSound;
}

const string& URConfigData::getDieSound() const
{
    return _dieSound;
}

float URConfigData::getBodyScale() const
{
    return _bodyScale;
}

float URConfigData::getEffectScale() const
{
    return _effectScale;
}

float URConfigData::getBodyEffectPosX() const
{
    return _bodyEffectPosX;
}

float URConfigData::getBodyEffectPosY() const
{
    return _bodyEffectPosY;
}

float URConfigData::getHeadEffectPosX() const
{
    return _headEffectPosX;
}

float URConfigData::getHeadEffectPosY() const
{
    return _headEffectPoxY;
}

float URConfigData::getHpBarPosX() const
{
    return _hpBarPosX;
}

float URConfigData::getHpBarPosY() const
{
    return _hpBarPosY;
}

float URConfigData::getHpBarScaleX() const
{
    return _hpBarScaleX;
}

float URConfigData::getMoveDuratioScale() const {
    return _moveDurationScale;
}

float URConfigData::getStandDurationScale() const {
    return _standDuraionScale;
}
