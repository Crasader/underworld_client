//
//  BattleDeckUnitNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/2/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "BattleDeckUnitNode.h"
#include "cocostudio/CocoStudio.h"
#include "CocosUtils.h"
#include "Camp.h"
#include "ResourceButton.h"
#include "DataManager.h"
#include "URConfigData.h"
#include "SoundManager.h"
#include "Spell.h"
#include "LocalHelper.h"

using namespace std;
using namespace cocostudio;
using namespace UnderWorld::Core;

static const int topZOrder(1);
static const Point iconTouchOffset(0, -6.0f);

BattleDeckUnitNode* BattleDeckUnitNode::create(const Camp* camp)
{
    BattleDeckUnitNode *ret = new (nothrow) BattleDeckUnitNode();
    if (ret && ret->init(camp))
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

BattleDeckUnitNode::BattleDeckUnitNode()
:_observer(nullptr)
,_cardWidget(nullptr)
,_addButton(nullptr)
,_iconSprite(nullptr)
,_qualitySprite(nullptr)
,_goldSprite(nullptr)
,_woodSprite(nullptr)
,_countLabel(nullptr)
,_goldLabel(nullptr)
,_woodLabel(nullptr)

,_shiningSprite(nullptr)
,_camp(nullptr)
,_idx(CC_INVALID_INDEX)
,_touchInvalid(false)
,_isIconTouched(false)
{
    
}

BattleDeckUnitNode::~BattleDeckUnitNode()
{
    removeAllChildren();
}

bool BattleDeckUnitNode::init(const Camp* camp)
{
    if (Node::init())
    {
        static const string csbFile("UI_Card.csb");
        Node *mainNode = CSLoader::createNode(csbFile);
        addChild(mainNode);
        
        timeline::ActionTimeline *action = CSLoader::createTimeline(csbFile);
        mainNode->runAction(action);
        action->gotoFrameAndPlay(0, false);
        
        _cardWidget = dynamic_cast<Widget *>(mainNode->getChildByTag(42));
        _cardWidget->setSwallowTouches(false);
        
        const Vector<Node*>& children = _cardWidget->getChildren();
        for (int i = 0; i < children.size(); ++i)
        {
            Node* child = children.at(i);
            if (child) {
                const int tag = child->getTag();
                switch (tag) {
                    case 44: {
                        _qualitySprite = Sprite::create("GameImages/test/ui_kuang_1.png");
                        child->addChild(_qualitySprite);
                    }
                        break;
                    case 45: {
                        const string& iconFile = getIconFile(camp, true);
                        _iconSprite = Sprite::create(iconFile);
                        child->addChild(_iconSprite);
                    }
                        break;
                    case 51: {
                        Node* node = child->getChildByTag(52);
                        if (node) {
                            _countLabel = CocosUtils::createLabel("0", DEFAULT_FONT_SIZE, DEFAULT_NUMBER_FONT);
                            node->addChild(_countLabel);
                        } else {
                            assert(false);
                        }
                    }
                        break;
                    case 49: {
                        _goldSprite = dynamic_cast<Sprite*>(child);
                        if (_goldSprite) {
                            _goldSprite->setLocalZOrder(topZOrder);
                        }
                        
                        Node* node = child->getChildByTag(50);
                        if (node) {
                            _goldLabel = CocosUtils::createLabel("0", DEFAULT_FONT_SIZE, DEFAULT_NUMBER_FONT);
                            node->addChild(_goldLabel);
                        } else {
                            assert(false);
                        }
                    }
                        break;
                    case 47: {
                        _woodSprite = dynamic_cast<Sprite*>(child);
                        if (_woodSprite) {
                            _woodSprite->setLocalZOrder(topZOrder);
                        }
                        
                        Node* node = child->getChildByTag(48);
                        if (node) {
                            _woodLabel = CocosUtils::createLabel("0", DEFAULT_FONT_SIZE, DEFAULT_NUMBER_FONT);
                            node->addChild(_woodLabel);
                        } else {
                            assert(false);
                        }
                    }
                        break;
                    case 53: {
                        const Vector<Node*>& children = child->getChildren();
                        for (int i = 0; i < children.size(); ++i)
                        {
                            Node* child = children.at(i);
                            if (child) {
                                const int tag = child->getTag();
                                static const int firstTag = 54;
                                if (tag >= firstTag && tag <= (firstTag + 4)) {
                                    Sprite* star = Sprite::create("GameImages/test/icon_xingxing.png");
                                    child->addChild(star);
                                    _starSprites.insert(make_pair(tag - firstTag, star));
                                }
                            }
                        }
                    }
                        break;
                    default:
                        break;
                }
            }
        }
        
        setContentSize(_cardWidget->getContentSize());
        
        _cardWidget->setSwallowTouches(false);
        _cardWidget->addTouchEventListener([=](Ref *pSender, Widget::TouchEventType type) {
            Widget* button = dynamic_cast<Widget*>(pSender);
            if (type == Widget::TouchEventType::BEGAN) {
                _isIconTouched = true;
                addTouchedAction(true, true);
                
                _touchInvalid = false;
                if(_observer) {
                    _observer->onBattleDeckUnitNodeTouchedBegan(_camp);
                }
            } else if (type == Widget::TouchEventType::MOVED) {
                if (!_touchInvalid) {
                    const Point& currentPoint = button->getTouchMovePosition();
                    const Point& beganPoint = button->getTouchBeganPosition();
                    if (abs(currentPoint.x - beganPoint.x) > 40.0f) {
                        _touchInvalid = true;
                    }
                }
            } else if (type == Widget::TouchEventType::ENDED) {
                _isIconTouched = false;
                addTouchedAction(false, true);
                
                if (!_touchInvalid) {
                    SoundManager::getInstance()->playButtonSound();
                }
                
                if(_observer) {
                    _observer->onBattleDeckUnitNodeTouchedEnded(_camp, !_touchInvalid);
                }
            } else {
                _isIconTouched = false;
                addTouchedAction(false, true);
                if(_observer) {
                    _observer->onBattleDeckUnitNodeTouchedCanceled(_camp);
                }
            }
        });
        
        // spell activated sprite
        {
            _shiningSprite = Sprite::create("GameImages/test/ui_xuanzhong.png");
            _shiningSprite->setVisible(false);
            _cardWidget->addChild(_shiningSprite);
            
            const Size& size = _cardWidget->getContentSize();
            _shiningSprite->setPosition(Point(size.width / 2, size.height / 2));
        }
        
        return true;
    }
    
    return false;
}

void BattleDeckUnitNode::registerObserver(BattleDeckUnitNodeObserver *observer)
{
    _observer = observer;
}

void BattleDeckUnitNode::reuse(const Camp* camp, ssize_t idx)
{
    _camp = camp;
    _idx = idx;
    
    // update mutable data
    update(true);
}

void BattleDeckUnitNode::update(bool reuse)
{
    if (_camp) {
        const int production = _camp->getProduction();
        const int maxProduction = _camp->getMaxProduction();
        const bool enable = production < maxProduction;
        const bool hero = isHero(_camp);
        
        if (!hero) {
            _countLabel->setString(StringUtils::format("%d/%d", production, maxProduction));
        }
        
        const map<string, int>& costs = _camp->getCosts();
        _woodSprite->setVisible(costs.find(RES_NAME_WOOD) != costs.end() && enable);
        if (_woodSprite->isVisible()) {
            int count = costs.at(RES_NAME_WOOD);
            _woodLabel->setString(StringUtils::format("%d", count));
        }
        
        _goldSprite->setVisible(costs.find(RES_NAME_GOLD) != costs.end() && enable);
        if (_goldSprite->isVisible()) {
            int count = costs.at(RES_NAME_GOLD);
            _goldLabel->setString(StringUtils::format("%d", count));
        }
        
        const string& iconFile = getIconFile(_camp, true);
        if (iconFile.length() > 0) {
            _iconSprite->setTexture(iconFile);
        }
    }
}

void BattleDeckUnitNode::setSelected(bool selected)
{
    if (_shiningSprite) {
        _shiningSprite->setVisible(selected);
    }
    
    static const float maxScale(1.1f);
    static const float minScale(1.0f);
    const float scale = _cardWidget->getScale();
    if (selected) {
        if (scale != maxScale) {
            _cardWidget->setScale(maxScale);
        }
    } else {
        if (scale != minScale) {
            _cardWidget->setScale(minScale);
        }
    }
}

void BattleDeckUnitNode::setTouched(bool touched, bool isGameOver)
{
    addTouchedAction(isGameOver ? _isIconTouched : touched, false);
}

const Camp* BattleDeckUnitNode::getCamp() const
{
    return _camp;
}

string BattleDeckUnitNode::getIconFile(const Camp* camp, bool enable) const
{
    const string& unitName = camp ? camp->getCurrentUnitType()->getRenderKey() : "GameImages/icons/unit/big/normal/icon_w_langdun.png";
    const URConfigData* configData = DataManager::getInstance()->getURConfigData(unitName);
    string iconFile;
    static const string defaultFile("GameImages/icons/unit/big/normal/icon_w_langdun.png");
    if (enable) {
        iconFile = configData ? configData->getIcon() : defaultFile;
    } else {
        iconFile = configData ? configData->getDisabledIcon() : defaultFile;
    }
    
    return iconFile;
}

bool BattleDeckUnitNode::isHero(const Camp* camp) const
{
    const UnitType* unitType = camp->getCurrentUnitType();
    if (kUnitClass_Hero == unitType->getUnitClass()) {
        return true;
    }
    
    return false;
}

void BattleDeckUnitNode::addTouchedAction(bool touched, bool animated)
{
    if (_cardWidget) {
        _cardWidget->stopAllActions();
    }
}
