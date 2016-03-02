//
//  MapUIUnitNode.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MapUIUnitNode.h"
#include "cocostudio/CocoStudio.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "Camp.h"
#include "ResourceButton.h"
#include "BattleSmallResourceNode.h"
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
,_iconSprite(nullptr)
,_qualitySprite(nullptr)
,_goldNode(nullptr)
,_shiningSprite(nullptr)
,_camp(nullptr)
,_touchInvalid(false)
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
        _camp = camp;
        
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
                        _qualitySprite = Sprite::create();
                        child->addChild(_qualitySprite);
                    }
                        break;
                    case 45: {
                        _iconSprite = Sprite::create();
                        child->addChild(_iconSprite);
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
                    case 58: {
                        _goldNode = BattleSmallResourceNode::create(kResourceType_Gold, 0);
                        child->addChild(_goldNode);
                    }
                        break;
                    default:
                        break;
                }
            }
        }
        
        const Size size(_cardWidget->getContentSize());
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        mainNode->setPosition(Point(size.width / 2, size.height / 2));
        
        _cardWidget->addTouchEventListener([=](Ref *pSender, Widget::TouchEventType type) {
            Widget* button = dynamic_cast<Widget*>(pSender);
            if (type == Widget::TouchEventType::BEGAN) {
                _touchInvalid = false;
                if(_observer) {
                    _observer->onMapUIUnitNodeTouchedBegan(_camp);
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
                if (!_touchInvalid) {
                    SoundManager::getInstance()->playButtonSound();
                }
                
                if(_observer) {
                    _observer->onMapUIUnitNodeTouchedEnded(_camp, !_touchInvalid);
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
        
        if (camp) {
            if (_qualitySprite) {
                _qualitySprite->setTexture(StringUtils::format("GameImages/test/ui_kuang_%d.png", camp->getBaseUnitType()->getRarity() + 1));
            }
            
            if (_iconSprite) {
                const string& iconFile = getIconFile(camp, true);
                if (iconFile.length() > 0) {
                    _iconSprite->setTexture(iconFile);
                }
            }
        }
        
        return true;
    }
    
    return false;
}

void MapUIUnitNode::registerObserver(MapUIUnitNodeObserver *observer)
{
    _observer = observer;
}

void MapUIUnitNode::update(int gold, int wood)
{
    if (_camp) {
        const int production = _camp->getProduction();
        const int maxProduction = _camp->getMaxProduction();
        const bool isNotFull = production < maxProduction;
        const bool hero = isHero(_camp);
        const Unit* heroUnit = _camp->getHero();
        
        bool colorful(true);
        const bool valid(_camp->getColdDown() <= 0);
        if (!hero || !heroUnit) {
            setResourceStatus(gold, isNotFull);
            
            if (isNotFull) {
                if (production < 1) {
                    colorful = false;
                } else {
                    colorful = valid;
                }
            } else {
                colorful = valid;
            }
        } else {
            if (heroUnit->isAlive()) {
                if (heroUnit->getSpellCount() == 0) {
                    colorful = false;
                }
            } else {
                colorful = valid;
            }
        }
        
        setColdDownProgress(colorful);
        
        // set icon
        const string& iconFile = getIconFile(_camp, colorful);
        if (iconFile.length() > 0) {
            _iconSprite->setTexture(iconFile);
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

const Camp* MapUIUnitNode::getCamp() const
{
    return _camp;
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

bool MapUIUnitNode::setResourceStatus(int count, bool enable)
{
    auto node = _goldNode;
    const string& name = RES_NAME_GOLD;
    int neededCount(0);
    const auto& costs = _camp->getCurrentPutCost();
    const bool isEnable(costs.find(name) != costs.end() && enable);
    if (isEnable) {
        neededCount = costs.at(name);
    }
    
    bool enoughResources(true);
    if (isEnable) {
        node->setCount(neededCount);
        node->check(count);
        if (count < neededCount) {
            enoughResources = false;
        }
    }
    
    return enoughResources;
}

void MapUIUnitNode::setColdDownProgress(bool colorful)
{
    const bool hero = isHero(_camp);
    const Unit* heroUnit = _camp->getHero();
    if (hero && heroUnit && heroUnit->isAlive()) {
        if (heroUnit->getSpellCount() > 0) {
            const Spell* spell = heroUnit->getSpellByIndex(0);
            if (spell) {
                float cd = spell->getCDProgress();
                _coldDownProgress->setVisible(cd > 0);
                _coldDownProgress->setPercentage(cd * 100.f / spell->getTotalCDFrames());
            }
        }
    } else {
        float cd = _camp->getColdDown();
        _coldDownProgress->setVisible(cd > 0);
        _coldDownProgress->setPercentage(cd * 100.f / _camp->getSpan());
    }
    
    if (colorful) {
        _coldDownProgress->setSprite(Sprite::create("GameImages/test/ui_iconzhezhao.png"));
    } else {
        _coldDownProgress->setSprite(Sprite::create("GameImages/test/ui_iconzhezhao_white.png"));
    }
}
