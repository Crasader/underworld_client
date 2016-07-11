//
//  URConfigData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "URConfigData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"

using namespace std;

static const string suffix(".csb");
static const string audioPrefix("sounds/effect/");
static const string audioSuffix(".mp3");

URConfigData::URConfigData(tinyxml2::XMLElement *xmlElement)
:_isShortRange(false)
,_isPVR(false)
,_isFaceRight(false)
,_footEffectPosition(Point::ZERO)
,_footEffectScaleX(1.0f)
,_footEffectScaleY(1.0f)
,_hpBarPosX(0.0f)
,_hpBarPosY(0.0f)
,_hpBarScaleX(1.0f)
{
    if (xmlElement) {
        _name = xmlElement->Attribute("render_key");
        {
            const char *data = xmlElement->Attribute("is_short_range");
            if (data && strlen(data) > 0) {
                _isShortRange = (atoi(data) != 0);
            } else {
                _isShortRange = false;
            }
        }
        {
            const char *data = xmlElement->Attribute("is_pvr");
            if (data && strlen(data) > 0) {
                _isPVR = (atoi(data) != 0);
            } else {
                _isPVR = false;
            }
        }
        {
            const char *data = xmlElement->Attribute("direction");
            if (data && strlen(data) > 0) {
                _isFaceRight = (atoi(data) != 0);
            } else {
                _isFaceRight = false;
            }
        }
        {
            const char *data = xmlElement->Attribute("prefix");
            if (data && strlen(data) > 0) {
                _prefix = data;
            }
        }
        {
            const char *data = xmlElement->Attribute("b_normal");
            if (data && strlen(data) > 0) {
                _bNormal = data + suffix;
            }
        }
        {
            const char *data = xmlElement->Attribute("b_damaged");
            if (data && strlen(data) > 0) {
                _bDamaged = data + suffix;
            }
        }
        {
            const char *data = xmlElement->Attribute("b_destroyed");
            if (data && strlen(data) > 0) {
                _bDestroyed = data + suffix;
            }
        }
        {
            const char *data = xmlElement->Attribute("sword_effect");
            if (data && strlen(data) > 0) {
                _swordEffect = data + suffix;
            }
        }
        {
            const char *data = xmlElement->Attribute("strike_point");
            if (data && strlen(data) > 0) {
                _hurtEffect = data + suffix;
            }
        }
        {
            const char *data = xmlElement->Attribute("crit_strike_point");
            if (data && strlen(data) > 0) {
                _critHurtEffect = data + suffix;
            }
        }
        {
            const char *data = xmlElement->Attribute("attck_sound");
            if (data && strlen(data) > 0) {
                _attckSound = audioPrefix + data + audioSuffix;
            }
        }
        {
            const char *data = xmlElement->Attribute("hurt_sound");
            if (data && strlen(data) > 0) {
                _hurtSound = audioPrefix + data + audioSuffix;
            }
        }
        {
            const char *data = xmlElement->Attribute("die_sound");
            if (data && strlen(data) > 0) {
                _dieSound = audioPrefix + data + audioSuffix;
            }
        }
        {
            const char *data = xmlElement->Attribute("foot_pos_x");
            if (data) {
                _footEffectPosition.x = atof(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("foot_pos_y");
            if (data) {
                _footEffectPosition.y = atof(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("foot_scale_x");
            if (data) {
                _footEffectScaleX = atof(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("foot_scale_y");
            if (data) {
                _footEffectScaleY = atof(data);
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
    }
}

URConfigData::~URConfigData()
{
    
}

const string& URConfigData::getName() const
{
    return _name;
}

bool URConfigData::isShortRange() const
{
    return _isShortRange;
}

bool URConfigData::isPVR() const
{
    return _isPVR;
}

bool URConfigData::isFaceRight() const
{
    return _isFaceRight;
}

const string& URConfigData::getPrefix() const
{
    return _prefix;
}

const string& URConfigData::getBNormal() const
{
    return _bNormal;
}

const string& URConfigData::getBDamaged() const
{
    return _bDamaged;
}

const string& URConfigData::getBDestroyed() const
{
    return _bDestroyed;
}

const string& URConfigData::getSwordEffect() const
{
    return _swordEffect;
}

const string& URConfigData::getHurtEffect() const
{
    return _hurtEffect;
}

const string& URConfigData::getCritHurtEffect() const
{
    return _critHurtEffect;
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

const Point& URConfigData::getFootEffectPosition() const
{
    return _footEffectPosition;
}

float URConfigData::getFootEffectScaleX() const
{
    return _footEffectScaleX;
}

float URConfigData::getFootEffectScaleY() const
{
    return _footEffectScaleY;
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
