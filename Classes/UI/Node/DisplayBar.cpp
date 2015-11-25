//
//  DisplayBar.cpp
//  Underworld_Client
//
//  Created by Andy on 15/11/25.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "DisplayBar.h"

DisplayBar* DisplayBar::create(DisplayBarType type, UnderWorld::Core::UnitClass unitClass)
{
    DisplayBar *ret = new (std::nothrow) DisplayBar();
    if (ret && ret->init(type, unitClass))
    {
        ret->autorelease();
        return ret;
    }
    else
    {
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
}

DisplayBar::DisplayBar()
:_pt(nullptr)
,_type(kHP)
,_unitClass(UnderWorld::Core::kUnitClass_Warrior)
{
    
}

DisplayBar::~DisplayBar()
{
    removeAllChildren();
}

void DisplayBar::setPercentage(float percentage)
{
    if (_pt) {
        const float lastPercentage = _pt->getPercentage();
        
        // if the unit is hurt
        if (!_visible && (percentage < 100 || lastPercentage < 100)) {
            setVisible(true);
        }
        
        if (_visible) {
            // need to change the sprite
            static const float threshold = 50;
            if ((threshold - lastPercentage) * (threshold - percentage) < 0) {
                std::string unused;
                std::string ptFile;
                getFiles((percentage < threshold) ? true : false, unused, ptFile);
                _pt->setSprite(Sprite::create(ptFile));
            }
            
            _pt->setPercentage(percentage);
        }
    }
}

float DisplayBar::getPercentage() const
{
    if (_pt) {
        return _pt->getPercentage();
    }
    
    return 0.0f;
}

bool DisplayBar::init(DisplayBarType type, UnderWorld::Core::UnitClass unitClass)
{
    if (Node::init()) {
        _type = type;
        _unitClass = unitClass;
        
        std::string bgFile;
        std::string ptFile;
        getFiles(false, bgFile, ptFile);
        
        Sprite* bg = Sprite::create(bgFile);
        addChild(bg);
        
        _pt = ProgressTimer::create(Sprite::create(ptFile));
        _pt->setType(ProgressTimer::Type::BAR);
        _pt->setBarChangeRate(Vec2(1.0f, 0.0f));
        _pt->setMidpoint(Point::ANCHOR_BOTTOM_LEFT);
        // set 100 by default
        _pt->setPercentage(100.0f);
        
        const Size& size = bg->getContentSize();
        _pt->setPosition(Point(size.width / 2, size.height / 2));
        bg->addChild(_pt);
        
        // hide first
        setVisible(false);
        
        return true;
    }
    
    return false;
}

void DisplayBar::getFiles(bool lowPercentage, std::string& bgFile, std::string& ptFile) const
{
    const bool isBig = (UnderWorld::Core::kUnitClass_Building == _unitClass || UnderWorld::Core::kUnitClass_Core == _unitClass) ? true : false;
    if (true) {
        if (isBig) {
            bgFile.assign("GameImages/test/ui_blood_3.png");
            ptFile.assign(lowPercentage ? "GameImages/test/ui_blood_5.png" : "GameImages/test/ui_blood_4.png");
        } else {
            bgFile.assign("GameImages/test/ui_blood.png");
            ptFile.assign(lowPercentage ? "GameImages/test/ui_blood_2.png" : "GameImages/test/ui_blood_1.png");
        }
    }
}