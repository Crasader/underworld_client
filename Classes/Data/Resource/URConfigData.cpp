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

#pragma mark =====================================================
#pragma mark Unit Resource Config Data
#pragma mark =====================================================

URConfigData::URConfigData(tinyxml2::XMLElement *xmlElement)
:_isShortRange(false)
{
    if (xmlElement) {
        _name = xmlElement->Attribute("unit_name");
        {
            const char *data = xmlElement->Attribute("is_short_range");
            if (data && strlen(data) > 0) {
                _isShortRange = (atoi(data) != 0);
            } else {
                _isShortRange = false;
            }
        }
        {
            const char *data = xmlElement->Attribute("icon_name");
            if (data && strlen(data) > 0) {
                static const string prefix("GameImages/test/");
                static const string suffix(".png");
                _icon = prefix + data + suffix;
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
            const char *data = xmlElement->Attribute("b_attack_begin");
            if (data && strlen(data) > 0) {
                _bAttackBegin = data + suffix;
            }
        }
        {
            const char *data = xmlElement->Attribute("b_attack");
            if (data && strlen(data) > 0) {
                _bAttack = data + suffix;
            }
        }
        {
            const char *data = xmlElement->Attribute("b_attack_end");
            if (data && strlen(data) > 0) {
                _bAttackEnd = data + suffix;
            }
        }
        {
            const char *data = xmlElement->Attribute("bullet");
            if (data && strlen(data) > 0) {
                _bullet = data + suffix;
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
    }
}

URConfigData::~URConfigData()
{
    
}

const std::string& URConfigData::getName() const
{
    return _name;
}

bool URConfigData::isShortRange() const
{
    return _isShortRange;
}

const std::string& URConfigData::getIcon() const
{
    return _icon;
}

const std::string& URConfigData::getPrefix() const
{
    return _prefix;
}

const std::string& URConfigData::getBNormal() const
{
    return _bNormal;
}

const std::string& URConfigData::getBDamaged() const
{
    return _bDamaged;
}

const std::string& URConfigData::getBDestroyed() const
{
    return _bDestroyed;
}

const std::string& URConfigData::getBAttackBegin() const
{
    return _bAttackBegin;
}

const std::string& URConfigData::getBAttack() const
{
    return _bAttack;
}

const std::string& URConfigData::getBAttackEnd() const
{
    return _bAttackEnd;
}

const std::string& URConfigData::getBullet() const
{
    return _bullet;
}

const std::string& URConfigData::getSwordEffect() const
{
    return _swordEffect;
}

const std::string& URConfigData::getHurtEffect() const
{
    return _hurtEffect;
}