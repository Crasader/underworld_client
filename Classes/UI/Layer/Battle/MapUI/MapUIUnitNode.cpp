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
                                    _observer->onMapUIUnitNodeClickedAddButton(_camp);
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
                                        
                                        static string file("UI-quan-2.csb");
                                        Node *effect = CSLoader::createNode(file);
                                        const Size& size = child->getContentSize();
                                        effect->setPosition(size.width / 2, size.height / 2);
                                        child->addChild(effect, -1);
                                        timeline::ActionTimeline *action = CSLoader::createTimeline(file);
                                        effect->runAction(action);
                                        action->gotoFrameAndPlay(0, true);
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
                                        
                                        static string file("UI-quan-1.csb");
                                        Node *effect = CSLoader::createNode(file);
                                        const Size& size = child->getContentSize();
                                        effect->setPosition(size.width / 2, size.height / 2);
                                        child->addChild(effect, -1);
                                        timeline::ActionTimeline *action = CSLoader::createTimeline(file);
                                        effect->runAction(action);
                                        action->gotoFrameAndPlay(0, true);
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
        
        return true;
    }
    
    return false;
}

void MapUIUnitNode::registerObserver(MapUIUnitNodeObserver *observer)
{
    _observer = observer;
}

void MapUIUnitNode::reuse(const Camp* camp, int gold, int wood)
{
    _camp = camp;
    
    // update mutable data
    update(true, gold, wood);
}

void MapUIUnitNode::update(bool reuse, int gold, int wood)
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
            if (!hero) {
                _countLabel->setString(StringUtils::format("%d/%d", production, maxProduction));
            }
            
            bool enoughResources = setResourceStatus(true, gold, isNotFull) && setResourceStatus(false, wood, isNotFull);
            _addButton->setVisible(isNotFull);
            _maxIconSprite->setVisible(!isNotFull);
            
            if (isNotFull) {
                _addButton->setEnabled(enoughResources);
                if (production < 1) {
                    colorful = false;
                } else {
                    colorful = valid;
                }
            } else {
                colorful = valid;
            }
        } else {
            if (!heroUnit->isAlive()) {
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

bool MapUIUnitNode::setResourceStatus(bool isGold, int count, bool enable)
{
    Sprite* sprite = isGold ? _goldSprite : _woodSprite;
    Label* label = isGold ? _goldLabel : _woodLabel;
    
    const map<string, int>& costs = _camp->getCosts();
    const string& name = isGold ? RES_NAME_GOLD : RES_NAME_WOOD;
    const bool isEnable(costs.find(name) != costs.end() && enable);
    sprite->setVisible(isEnable);
    
    bool enoughResources(true);
    if (isEnable) {
        static const Color4B& enabledColor(Color4B::WHITE);
        static const Color4B& disabledColor(Color4B::RED);
        const int neededCount = costs.at(name);
        label->setString(StringUtils::format("%d", neededCount));
        if (count < neededCount) {
            enoughResources = false;
            label->setTextColor(disabledColor);
        } else {
            label->setTextColor(enabledColor);
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
