//
//  HMMCardView.cpp
//  Underworld_Client
//
//  Created by wenchengye on 16/7/30.
//
//

#include "HMMCardView.h"
#include "GameModeHMM.h"
#include "DataManager.h"
#include "BattleSmallResourceNode.h"
#include "CardConfigData.h"
#include "CocosUtils.h"
#include "CocosGlobal.h"
#include "CoreUtils.h"

namespace UnderWorld{ namespace Core{
    
const std::string HMMCardView::DISPLAY_RESOURCE_KEY = RES_NAME_WOOD;
    
HMMCardView* HMMCardView::create(const HMMCardType* cardType) {
    HMMCardView* ret = new (std::nothrow) HMMCardView();
    if (ret && ret->init(cardType)) {
        ret->autorelease();
    } else {
        CC_SAFE_DELETE(ret);
    }
    return ret;
}
    
HMMCardView::~HMMCardView() {
    this->removeAllChildren();
}
    
bool HMMCardView::init(const HMMCardType* cardType) {
    static const float card_width = 80.f;
    static const float card_height = 105.f;
    
    static const int quality_zorder = 1;
    static const int icon_zorder = 2;
    static const int progress_zorder = 3;
    static const int shining_zorder = 4;
    static const int resource_zorder = 5;
    
    if (!cocos2d::Node::init()) return false;
    
    /** init data */
    _cardType = nullptr;
    _configData = nullptr;
    
    _cardType = cardType;
    if (_cardType) _configData = DataManager::getInstance()->getCardConfigData(_cardType->getId());
    
    /** init cocos */
    //1. clean up
    this->removeAllChildren();
    _iconSprite = nullptr;
    _qualitySprite = nullptr;
    _resourceNode = nullptr;
    _shiningSprite = nullptr;
    _coldDownProgress = nullptr;
    
    //2. init views
    this->setCascadeOpacityEnabled(true);
    this->setContentSize(cocos2d::Size(card_width, card_height));
    
    //quality
    //TODO: add quality in card type
    _qualitySprite = cocos2d::Sprite::create();
    _qualitySprite->setPosition(cocos2d::Vec2(card_width / 2, card_height / 2));
    this->addChild(_qualitySprite, quality_zorder);
    
    // icon
    _iconSprite = cocos2d::Sprite::create();
    _iconSprite->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    this->addChild(_iconSprite, icon_zorder);
    
    // shining
    _shiningSprite = Sprite::create("GameImages/battle_ui/ui_card_shing.png");
    if (_shiningSprite) {
        _shiningSprite->setVisible(false);
        _shiningSprite->setPosition(cocos2d::Vec2(card_width / 2, card_height / 2));
        this->addChild(_shiningSprite, shining_zorder);
    }
    
    // resource
    _resourceNode = BattleSmallResourceNode::create(core_resource_type_key_2_ui_resource_type(DISPLAY_RESOURCE_KEY), 0);
    if (_resourceNode) {
        _resourceNode->setPosition(cocos2d::Vec2(card_width / 2, card_height));
        this->addChild(_resourceNode, resource_zorder);
    }
    
    //  CD
    _coldDownProgress = cocos2d::ProgressTimer::create(cocos2d::Sprite::create("GameImages/battle_ui/ui_card_colddown_content.png"));
    if (_coldDownProgress) {
        _coldDownProgress->setType(cocos2d::ProgressTimer::Type::RADIAL);
        _coldDownProgress->setReverseDirection(true);
        _coldDownProgress->setMidpoint(cocos2d::Vec2::ANCHOR_MIDDLE);
        _coldDownProgress->setPosition(cocos2d::Vec2(card_width / 2, card_height / 2));
        this->addChild(_coldDownProgress, progress_zorder);
    }
    
    //3. fill views
    _currentIconFile.clear();
    if (_iconSprite && _configData) {
        _currentIconFile = _configData->getIcon();
        _iconSprite->setVisible(!_currentIconFile.empty());
        if (!_currentIconFile.empty()) {
            _iconSprite->setTexture(_currentIconFile);
        }
    }
    
    //TODO: rarity in cardType
    int rarity = 0;
    if (_cardType && _qualitySprite) {
        _qualitySprite->setTexture(CocosUtils::getResourcePath("ui_quality_box_0.png"));
    }
    
    if (_resourceNode && _cardType) {
        auto iter = _cardType->getCost().find(DISPLAY_RESOURCE_KEY);
        int cost = iter == _cardType->getCost().end() ? 0 : GameConstants::microres2Res(iter->second);
        _resourceNode->setCount(cost);
        _resourceNode->check(cost);
        _resourceNode->setVisible(cost != 0);
    }
    
    if (_shiningSprite) {
        _shiningSprite->setVisible(false);
    }
    
    if (_coldDownProgress) {
        _coldDownProgress->setPercentage(0);
        _coldDownProgress->setVisible(false);
    }
    
    return true;
}
    
void HMMCardView::setCardUseable(bool useable) {
    if (!_iconSprite || !_configData) return;
    const std::string& iconFile = useable ? _configData->getIcon() : _configData->getDisabledIcon();
    if (iconFile != _currentIconFile) {
        _currentIconFile = iconFile;
        _iconSprite->setVisible(!_currentIconFile.empty());
        if (!_currentIconFile.empty()) _iconSprite->setTexture(_currentIconFile);
    }
}

void HMMCardView::setResourceEnough(microres_t amount) {
    if (_resourceNode) {
        _resourceNode->check(GameConstants::microres2Res(amount));
    }
}
    
void HMMCardView::setSelected(bool selected) {
    if (_shiningSprite) {
        _shiningSprite->setVisible(selected);
    }
}
    
void HMMCardView::setColdDownProgress(int progress) {
    if (!_coldDownProgress) return;
    
    _coldDownProgress->setVisible(progress > 0);
    _coldDownProgress->setPercentage(progress);
}
    
HMMCardView::HMMCardView()
: _cardType(nullptr)
, _configData(nullptr)
, _iconSprite(nullptr)
, _qualitySprite(nullptr)
, _resourceNode(nullptr)
, _shiningSprite(nullptr)
, _coldDownProgress(nullptr) {
}
    
}}