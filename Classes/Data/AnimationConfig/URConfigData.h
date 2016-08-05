//
//  URConfigData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef URConfigData_h
#define URConfigData_h

#include "cocos2d.h"

USING_NS_CC;

namespace tinyxml2 { class XMLElement; }

enum class UnitAnimationType {
    PVR,
    CSB,
    
    UNIT_ANIMATION_TYPE_COUNT
};

class URConfigData
{
public:
    URConfigData(tinyxml2::XMLElement *xmlElement);
    virtual ~URConfigData();
    
    const std::string& getName() const;
    bool isFaceRight() const;
    bool isMultiDirection() const;
    bool isTransformOnHealth() const;
    UnitAnimationType getAnimationType() const;
    const std::string& getNormalPrefix() const;
    const std::string& getAttackSound() const;
    const std::string& getHurtSound() const;
    const std::string& getDieSound() const;
    
    float getBodyScale() const;
    float getEffectScale() const;
    float getBodyEffectPosX() const;
    float getBodyEffectPosY() const;
    float getHeadEffectPosX() const;
    float getHeadEffectPosY() const;
    
    float getHpBarPosX() const;
    float getHpBarPosY() const;
    float getHpBarScaleX() const;
    
    float getMoveDuratioScale() const;
    float getStandDurationScale() const;
    
private:
    std::string _name;
    bool _isFaceRight;
    bool _isMultiDirection;
    bool _isTransformOnHealth;
    UnitAnimationType _animationType;
    std::string _normalPrefix;
    std::string _attckSound;
    std::string _hurtSound;
    std::string _dieSound;
    
    float _bodyScale;
    float _effectScale;
    float _bodyEffectPosX;
    float _bodyEffectPosY;
    float _headEffectPosX;
    float _headEffectPoxY;
    
    float _hpBarPosX;
    float _hpBarPosY;
    float _hpBarScaleX;
    
    float _moveDurationScale;
    float _standDuraionScale;
};

#endif /* URConfigData_h */
