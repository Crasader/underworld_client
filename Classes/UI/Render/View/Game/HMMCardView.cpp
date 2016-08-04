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
    static const std::string card_csb_file("UI_Card.csb");
    
    if (!cocos2d::Node::init()) return false;
    
    /** init data */
    _cardType = nullptr;
    _configData = nullptr;
    
    _cardType = cardType;
    if (_cardType) _configData = DataManager::getInstance()->getCardConfigData(_cardType->getName());
    
    /** init cocos */
    //1. clean up
    this->removeAllChildren();
    _cardWidget = nullptr;
    _iconSprite = nullptr;
    _qualitySprite = nullptr;
    _resourceNode = nullptr;
    _shiningSprite = nullptr;
    _coldDownProgress = nullptr;
    
    //2. init views
    this->setCascadeOpacityEnabled(true);
    cocos2d::Node* mainNode = CocosUtils::playAnimation(card_csb_file, 0.f, false, 0, -1, nullptr);
    if (mainNode) {
        mainNode->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
        this->addChild(mainNode);
        _cardWidget = dynamic_cast<cocos2d::ui::Widget *>(mainNode->getChildByTag(42));
    }
    
    if (_cardWidget) {
        _cardWidget->setSwallowTouches(false);
        
        const cocos2d::Vector<cocos2d::Node*>& children = _cardWidget->getChildren();
        for (int i = 0; i < children.size(); ++i) {
            Node* child = children.at(i);
            if (child) {
                const int tag = child->getTag();
                switch (tag) {
                    case 44: {
                        _qualitySprite = cocos2d::Sprite::create();
                        child->addChild(_qualitySprite);
                    }
                        break;
                    case 45: {
                        _iconSprite = cocos2d::Sprite::create();
                        child->addChild(_iconSprite);
                    }
                        break;
                    case 58: {
                        _resourceNode = BattleSmallResourceNode::create(core_resource_type_key_2_ui_resource_type(DISPLAY_RESOURCE_KEY), 0);
                        child->addChild(_resourceNode);
                    }
                        break;
                    default:
                        break;
                }
            }
        }
        
        mainNode->setContentSize(_cardWidget->getContentSize());
        this->setContentSize(_cardWidget->getContentSize());
        
        //  CD
        {
            _coldDownProgress = cocos2d::ProgressTimer::create(cocos2d::Sprite::create("GameImages/test/ui_iconzhezhao_white.png"));
            if (_coldDownProgress) {
                _coldDownProgress->setType(cocos2d::ProgressTimer::Type::RADIAL);
                _coldDownProgress->setReverseDirection(true);
                _coldDownProgress->setMidpoint(cocos2d::Vec2::ANCHOR_MIDDLE);
                _coldDownProgress->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
                _cardWidget->addChild(_coldDownProgress);
                
            }
        }
        
        // activated sprite
        {
            _shiningSprite = Sprite::create("GameImages/test/ui_xuanzhong.png");
            if (_shiningSprite) {
                _shiningSprite->setVisible(false);
                _shiningSprite->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
                _cardWidget->addChild(_shiningSprite);
            }
        }
    }
    
    //3. fill views
    _currentIconFile.clear();
    if (_iconSprite && _configData) {
        _currentIconFile = _configData->getIcon();
        _iconSprite->setVisible(_currentIconFile.empty());
        if (!_currentIconFile.empty()) {
            _iconSprite->setTexture(_currentIconFile);
        }
    }
    
    //TODO: rarity in cardType
    int rarity = 0;
    if (_cardType && _qualitySprite) {
        _qualitySprite->setTexture("GameImages/test/ui_kuang_"+ std::to_string(rarity + 1) + ".png");
    }
    
    if (_resourceNode && _cardType) {
        auto iter = _cardType->getCost().find(DISPLAY_RESOURCE_KEY);
        int cost = iter == _cardType->getCost().end() ? 0 : iter->second;
        _resourceNode->setCount(cost);
        _resourceNode->check(cost);
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
, _cardWidget(nullptr)
, _iconSprite(nullptr)
, _qualitySprite(nullptr)
, _resourceNode(nullptr)
, _shiningSprite(nullptr)
, _coldDownProgress(nullptr) {
}
    
}}