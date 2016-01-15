//
//  URConfigData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef URConfigData_h
#define URConfigData_h

#include <iostream>
#include "cocos2d.h"

USING_NS_CC;

namespace tinyxml2 { class XMLElement; }

// =====================================================
// Unit Resource Config Data
// =====================================================

class URConfigData
{
public:
    URConfigData(tinyxml2::XMLElement *xmlElement);
    virtual ~URConfigData();
    
    const std::string& getName() const;
    bool isShortRange() const;
    bool isFaceRight() const;
    const std::string& getIcon() const;
    const std::string& getSmallIcon() const;
    const std::string& getPrefix() const;
    const std::string& getBNormal() const;
    const std::string& getBDamaged() const;
    const std::string& getBDestroyed() const;
    const std::string& getBAttackBegin() const;
    const std::string& getBAttack() const;
    const std::string& getBAttackEnd() const;
    const std::string& getBullet() const;
    const std::string& getCritBullet() const;
    const std::string& getSwordEffect() const;
    const std::string& getHurtEffect() const;
    const std::string& getCritHurtEffect() const;
    const std::string& getAttackSound() const;
    const std::string& getHurtSound() const;
    const std::string& getDieSound() const;
    
    const Point& getBodyEffectPosition() const;
    float getBodyEffectScaleX() const;
    float getBodyEffectScaleY() const;
    
    const Point& getFootEffectPosition() const;
    float getFootEffectScaleX() const;
    float getFootEffectScaleY() const;
    
    float getHpBarPosY() const;
    
private:
    std::string _name;
    bool _isShortRange;
    bool _isFaceRight;
    std::string _icon;
    std::string _smallIcon;
    std::string _prefix;
    std::string _bNormal;
    std::string _bDamaged;
    std::string _bDestroyed;
    std::string _bAttackBegin;
    std::string _bAttack;
    std::string _bAttackEnd;
    std::string _bullet;
    std::string _critBullet;
    std::string _swordEffect;
    std::string _hurtEffect;
    std::string _critHurtEffect;
    std::string _attckSound;
    std::string _hurtSound;
    std::string _dieSound;
    
    Point _bodyEffectPosition;
    float _bodyEffectScaleX;
    float _bodyEffectScaleY;
    
    Point _footEffectPosition;
    float _footEffectScaleX;
    float _footEffectScaleY;
    
    float _hpBarPosY;
};

#endif /* URConfigData_h */
