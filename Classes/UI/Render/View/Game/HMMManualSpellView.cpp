//
//  HMMManualSpellView.cpp
//  Underworld_Client
//
//  Created by Mac on 16/9/13.
//
//

#include "HMMManualSpellView.h"
#include "Spell.h"


namespace UnderWorld{ namespace Core{
    
HMMManualSpellView* HMMManualSpellView::create(const Spell* spell) {
    HMMManualSpellView* ret = new (std::nothrow) HMMManualSpellView();
    if (ret && ret->init(spell)) {
        ret->autorelease();
    } else {
        CC_SAFE_DELETE(ret);
    }
    return ret;
}
    
    HMMManualSpellView::~HMMManualSpellView() {
        this->removeAllChildren();
    }
    
bool HMMManualSpellView::init(const Spell *spell) {
    static const int icon_zorder = 1;
    static const int progress_zorder = 2;
    
    if (!cocos2d::Node::init()) return false;
    
    /** init data */
    _spell = spell;
    
    /** init cocos */
    //1. clean up
    this->removeAllChildren();
    _iconSprite = nullptr;
    _coldDownProgress = nullptr;
    
    //2. init views
    this->setCascadeOpacityEnabled(true);
    
    // icon
    _iconSprite = cocos2d::Sprite::create();
    this->addChild(_iconSprite, icon_zorder);
    
    // cd
    if (_coldDownProgress) {
        _coldDownProgress->setType(cocos2d::ProgressTimer::Type::RADIAL);
        _coldDownProgress->setReverseDirection(true);
        _coldDownProgress->setMidpoint(cocos2d::Vec2::ANCHOR_MIDDLE);
        this->addChild(_coldDownProgress, progress_zorder);
    }
    
    //3. fill views
    _currentIconFile.clear();
    if (_spell && spell->getSpellType()) {
        _currentIconFile.assign(spell->getSpellType()->getSpellIcon());
        if (!_currentIconFile.empty()) {
            _iconSprite->setTexture(_currentIconFile);
        }
    }
    
    if (_coldDownProgress) {
        _coldDownProgress->setPercentage(0);
        _coldDownProgress->setVisible(false);
    }
    
    this->setContentSize(_iconSprite ? _iconSprite->getContentSize() : cocos2d::Size::ZERO);
    cocos2d::Vec2 center(this->getContentSize().width / 2, this->getContentSize().height / 2);
    
    if (_iconSprite) _iconSprite->setPosition(center);
    if (_coldDownProgress) _coldDownProgress->setPosition(center);
    
    return true;
}
    
void HMMManualSpellView::setSpellUseable(bool useable) {
    if (!_iconSprite || !_spell) return;
}
    
void HMMManualSpellView::setColdDownProgress(int progress) {
    if (!_coldDownProgress) return;
    
    _coldDownProgress->setVisible(progress > 0);
    _coldDownProgress->setPercentage(progress);
}
    
HMMManualSpellView::HMMManualSpellView()
: _spell(nullptr)
, _iconSprite(nullptr)
, _coldDownProgress(nullptr) {
}
    
}}
