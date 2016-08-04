//
//  DisplayBar.cpp
//  Underworld_Client
//
//  Created by Mac on 16/7/19.
//
//

#include "DisplayBar.h"
    
// ===============================
// 	class DisplayBar
// ===============================

// ===================== public ========================

DisplayBar* DisplayBar::create(const std::string& background, const std::string& foreground) {
    DisplayBar* ret = new (std::nothrow) DisplayBar();
    
    if (ret && ret->init(background, foreground)) {
        ret->autorelease();
    } else {
        CC_SAFE_DELETE(ret);
    }
    
    return ret;
}
    
DisplayBar::~DisplayBar() {
    removeAllChildren();
}

// ===================== interface ========================

void DisplayBar::setPercentage(float percentage) {
    if (_pt) _pt->setPercentage(percentage);
}

void DisplayBar::setBackground(const std::string& background) {
    if (_bg) {
        _bg->setTexture(background);
        _backgroundFile.assign(background);
    }
    
    if (_bg && _pt) {
        const cocos2d::Size& size = _bg->getContentSize();
        _pt->setPosition(cocos2d::Vec2(size.width / 2, size.height / 2));
    }
}

void DisplayBar::setForeground(const std::string& foreground) {
    if (_pt) {
        cocos2d::Sprite* fg = cocos2d::Sprite::create(foreground);
        if (fg) {
            _pt->setSprite(fg);
            _foregroundFile.assign(foreground);
        }
    }
}

// ===================== protected ========================
    
DisplayBar::DisplayBar()
: _pt(nullptr)
, _bg(nullptr) {
    
}
    
bool DisplayBar::init(const std::string &background, const std::string &foreground) {
    if (!Node::init()) return false;

    this->removeAllChildren();
    _backgroundFile.clear();
    _foregroundFile.clear();
    
    _bg = cocos2d::Sprite::create(background);
    if (!_bg) return false;
    
    addChild(_bg);
    
    cocos2d::Sprite* fg = cocos2d::Sprite::create(foreground);
    if (!fg) return false;
    _pt = cocos2d::ProgressTimer::create(fg);
    _pt->setType(cocos2d::ProgressTimer::Type::BAR);
    _pt->setBarChangeRate(cocos2d::Vec2(1.0f, 0.0f));
    _pt->setMidpoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    _pt->setPercentage(100.f);
    
    const cocos2d::Size& size = _bg->getContentSize();
    _pt->setPosition(cocos2d::Vec2(size.width / 2, size.height / 2));
    _bg->addChild(_pt);
    
    _backgroundFile.assign(background);
    _foregroundFile.assign(foreground);
    
    return true;
}