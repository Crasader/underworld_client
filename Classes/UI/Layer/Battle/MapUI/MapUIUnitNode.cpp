//
//  MapUIUnitNode.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MapUIUnitNode.h"
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

static const int bottomZOrder(-1);
static const int topZOrder(1);
static const Point iconTouchOffset(0, -6.0f);

MapUIUnitNode* MapUIUnitNode::create(const Camp* camp)
{
    MapUIUnitNode *ret = new (nothrow) MapUIUnitNode();
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

MapUIUnitNode::MapUIUnitNode()
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
,_maxIconSprite(nullptr)
,_camp(nullptr)
,_idx(CC_INVALID_INDEX)
,_touchInvalid(false)
,_isIconTouched(false)
{
    
}

MapUIUnitNode::~MapUIUnitNode()
{
    removeAllChildren();
}

bool MapUIUnitNode::init(const Camp* camp)
{
    if (Node::init())
    {
        static const string csbFile("UI_BattleCard.csb");
        Node *mainNode = CSLoader::createNode(csbFile);
        addChild(mainNode);
        timeline::ActionTimeline *action = CSLoader::createTimeline(csbFile);
        mainNode->runAction(action);
        action->gotoFrameAndPlay(0, false);
        Widget* root = dynamic_cast<Widget *>(mainNode->getChildByTag(4));
        root->setSwallowTouches(false);
        const Vector<Node*>& children = root->getChildren();
        for (int i = 0; i < children.size(); ++i)
        {
            Node* child = children.at(i);
            if (child) {
                const int tag = child->getTag();
                switch (tag) {
                    case 20: {
                        Button* button = dynamic_cast<Button*>(child);
                        if (button) {
                            button->setPressedActionEnabled(true);
                            button->addClickEventListener([this](Ref *pSender){
                                SoundManager::getInstance()->playButtonSound();
                                if (_observer) {
                                    _observer->onMapUIUnitNodeClickedAddButton(this);
                                }
                            });
                            
                            _addButton = button;
                            
                            Sprite* sprite = Sprite::create("GameImages/test/button_yellow_4.png");
                            sprite->setPosition(child->getPosition());
                            child->getParent()->addChild(sprite);
                            
                            _maxIconSprite = sprite;
                        } else {
                            assert(false);
                        }
                    }
                        break;
                    case 21: {
                        Button* button = dynamic_cast<Button*>(child);
                        if (button) {
                            button->setPressedActionEnabled(true);
                            button->addClickEventListener([this](Ref *pSender){
                                SoundManager::getInstance()->playButtonSound();
                                if (_observer) {
                                    _observer->onMapUIUnitNodeClickedUpgradeButton(this);
                                }
                            });
                        } else {
                            assert(false);
                        }
                    }
                        break;
                    case 23: {
                        _cardWidget = dynamic_cast<Widget*>(child);
                        
                        const Vector<Node*>& children = child->getChildren();
                        for (int i = 0; i < children.size(); ++i)
                        {
                            Node* child = children.at(i);
                            if (child) {
                                const int tag = child->getTag();
                                switch (tag) {
                                    case 6: {
                                        _qualitySprite = Sprite::create("GameImages/test/ui_kuang_1.png");
                                        child->addChild(_qualitySprite);
                                    }
                                        break;
                                    case 7: {
                                        const string& iconFile = getIconFile(camp, true);
                                        _iconSprite = Sprite::create(iconFile);
                                        child->addChild(_iconSprite);
                                    }
                                        break;
                                    case 8: {
                                        Node* node = child->getChildByTag(10);
                                        if (node) {
                                            _countLabel = CocosUtils::createLabel("0", DEFAULT_FONT_SIZE, DEFAULT_NUMBER_FONT);
                                            node->addChild(_countLabel);
                                            child->setVisible(!isHero(camp));
                                        } else {
                                            assert(false);
                                        }
                                    }
                                        break;
                                    case 9: {
                                        _goldSprite = dynamic_cast<Sprite*>(child);
                                        if (_goldSprite) {
                                            _goldSprite->setLocalZOrder(topZOrder);
                                        }
                                        
                                        Node* node = child->getChildByTag(11);
                                        if (node) {
                                            _goldLabel = CocosUtils::createLabel("0", DEFAULT_FONT_SIZE, DEFAULT_NUMBER_FONT);
                                            node->addChild(_goldLabel);
                                        } else {
                                            assert(false);
                                        }
                                    }
                                        break;
                                    case 12: {
                                        _woodSprite = dynamic_cast<Sprite*>(child);
                                        if (_woodSprite) {
                                            _woodSprite->setLocalZOrder(topZOrder);
                                        }
                                        
                                        Node* node = child->getChildByTag(13);
                                        if (node) {
                                            _woodLabel = CocosUtils::createLabel("0", DEFAULT_FONT_SIZE, DEFAULT_NUMBER_FONT);
                                            node->addChild(_woodLabel);
                                        } else {
                                            assert(false);
                                        }
                                    }
                                        break;
                                    case 14: {
                                        const Vector<Node*>& children = child->getChildren();
                                        for (int i = 0; i < children.size(); ++i)
                                        {
                                            Node* child = children.at(i);
                                            if (child) {
                                                const int tag = child->getTag();
                                                if (tag >= 15 && tag <= 19) {
                                                    Sprite* star = Sprite::create("GameImages/test/icon_xingxing.png");
                                                    child->addChild(star);
                                                    _starSprites.insert(make_pair(tag - 15, star));
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
                    }
                        break;
                    default:
                        break;
                }
            }
        }
        
        setContentSize(root->getContentSize());
        
        _cardWidget->setSwallowTouches(false);
        _cardWidget->addTouchEventListener([=](Ref *pSender, Widget::TouchEventType type) {
            Widget* button = dynamic_cast<Widget*>(pSender);
            if (type == Widget::TouchEventType::BEGAN) {
                _isIconTouched = true;
                addTouchedAction(true, true);
                
                _touchInvalid = false;
                if(_observer) {
                    _observer->onMapUIUnitNodeTouchedBegan(this);
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
                    _observer->onMapUIUnitNodeTouchedEnded(this, !_touchInvalid);
                }
            } else {
                _isIconTouched = false;
                addTouchedAction(false, true);
                if(_observer) {
                    _observer->onMapUIUnitNodeTouchedCanceled(this);
                }
            }
        });
        
        // spell CD
        {
            _coldDownProgress = ProgressTimer::create(Sprite::create("GameImages/test/ui_iconzhezhao.png"));
            _coldDownProgress->setType(ProgressTimer::Type::RADIAL);
            _coldDownProgress->setReverseDirection(true);
            _coldDownProgress->setMidpoint(Point::ANCHOR_MIDDLE);
            _iconSprite->addChild(_coldDownProgress, topZOrder);
            
            const Size& size = _iconSprite->getContentSize();
            _coldDownProgress->setPosition(Point(size.width / 2, size.height / 2));
        }
        
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

void MapUIUnitNode::registerObserver(MapUIUnitNodeObserver *observer)
{
    _observer = observer;
}

void MapUIUnitNode::reuse(const Camp* camp, ssize_t idx, int gold, int wood)
{
    _camp = camp;
    _idx = idx;
    
    // update mutable data
    update(true, gold, wood);
}

void MapUIUnitNode::update(bool reuse, int gold, int wood)
{
    if (_camp) {
        const int production = _camp->getProduction();
        const int maxProduction = _camp->getMaxProduction();
        const bool enable = production < maxProduction;
        const bool hero = isHero(_camp);
        const Unit* heroUnit = _camp->getHero();
        
        if (!hero) {
            _countLabel->setString(StringUtils::format("%d/%d", production, maxProduction));
        }
        
        bool colorful(true);
        const map<string, int>& costs = _camp->getCosts();
        _woodSprite->setVisible(costs.find(RES_NAME_WOOD) != costs.end() && enable);
        if (_woodSprite->isVisible()) {
            const int count = costs.at(RES_NAME_WOOD);
            _woodLabel->setString(StringUtils::format("%d", count));
            if (wood < count) {
                colorful = false;
            }
        }
        
        _goldSprite->setVisible(costs.find(RES_NAME_GOLD) != costs.end() && enable);
        if (_goldSprite->isVisible()) {
            const int count = costs.at(RES_NAME_GOLD);
            _goldLabel->setString(StringUtils::format("%d", count));
            if (gold < count) {
                colorful = false;
            }
        }
        
        _addButton->setVisible(enable);
        _maxIconSprite->setVisible(!enable);
        
        if (hero) {
            if (colorful) {
                if (heroUnit) {
                    if (!heroUnit->isAlive()) {
                        colorful = false;
                    }
                } else {
                    const bool hasResources = _woodSprite->isVisible() || _goldSprite->isVisible();
                    if (!hasResources) {
                        colorful = false;
                    }
                }
                
                if (!colorful) {
                    _countLabel->setString(LocalHelper::getString("battle_summoning"));
                } else {
                    _countLabel->setString("");
                }
            } else {
                _countLabel->setString("");
            }
        }
        
        const string& iconFile = getIconFile(_camp, colorful);
        if (iconFile.length() > 0) {
            _iconSprite->setTexture(iconFile);
        }
        
        float cd(10);
        const bool isSpell(heroUnit && heroUnit->getSpellCount() > 0);
        if (isSpell) {
            cd = heroUnit->getSpellByIndex(0)->getCDProgress();
        } else {
            cd = _camp->getColdDown();
        }
        
        if (cd <= 0) {
            _coldDownProgress->setVisible(false);
        } else {
            _coldDownProgress->setVisible(true);
            if (colorful) {
                _coldDownProgress->setSprite(Sprite::create("GameImages/test/ui_iconzhezhao.png"));
            } else {
                _coldDownProgress->setSprite(Sprite::create("GameImages/test/ui_iconzhezhao_white.png"));
            }
            
            if (isSpell) {
                _coldDownProgress->setPercentage(cd * 100.f / heroUnit->getSpellByIndex(0)->getTotalCDFrames());
            } else {
                _coldDownProgress->setPercentage(cd * 100.f / _camp->getSpan());
            }
        }
        
        // add effect
        if (_iconSprite) {
#if true
            if (heroUnit && heroUnit->isAlive() && cd == 0) {
                _shiningSprite->setVisible(true);
            } else {
                _shiningSprite->setVisible(false);
            }
#else
            static const int spellActivatedTag(2016);
            Node* spellActivatedNode = getChildByTag(spellActivatedTag);
            if (heroUnit && heroUnit->isAlive() && spellCD == 0) {
                if (!spellActivatedNode) {
                    static const string file("kapai-UI.csb");
                    spellActivatedNode = CSLoader::createNode(file);
                    spellActivatedNode->setPosition(_iconBasePosition);
                    addChild(spellActivatedNode, bottomZOrder, spellActivatedTag);
                    timeline::ActionTimeline *action = CSLoader::createTimeline(file);
                    spellActivatedNode->runAction(action);
                    action->gotoFrameAndPlay(0, true);
                }
            } else if (spellActivatedNode) {
                spellActivatedNode->stopAllActions();
                spellActivatedNode->removeFromParent();
            }
#endif
        }
    }
}

void MapUIUnitNode::setSelected(bool selected)
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

void MapUIUnitNode::setTouched(bool touched, bool isGameOver)
{
    addTouchedAction(isGameOver ? _isIconTouched : touched, false);
}

const Camp* MapUIUnitNode::getCamp() const
{
    return _camp;
}

ssize_t MapUIUnitNode::getIdx() const
{
    return _idx;
}

string MapUIUnitNode::getIconFile(const Camp* camp, bool enable) const
{
    const string& unitName = camp ? camp->getCurrentUnitType()->getRenderKey() : "";
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

bool MapUIUnitNode::isHero(const Camp* camp) const
{
    if (camp) {
        const UnitType* unitType = camp->getCurrentUnitType();
        if (kUnitClass_Hero == unitType->getUnitClass()) {
            return true;
        }
    }
    
    return false;
}

void MapUIUnitNode::addTouchedAction(bool touched, bool animated)
{
    if (_cardWidget) {
        _cardWidget->stopAllActions();
    }
}
