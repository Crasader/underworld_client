//
//  CardNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/2/22.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "CardNode.h"
#include "cocostudio/CocoStudio.h"
#include "CocosUtils.h"
#include "BattleSmallResourceNode.h"
#include "DataManager.h"
#include "CardConfigData.h"
#include "GameModeHMM.h"
#include "CCShake.h"
#include "SoundManager.h"

using namespace std;
using namespace cocostudio;
using namespace UnderWorld::Core;

static const int topZOrder(1);
static const Point iconTouchOffset(0, -6.0f);
static const float normalScale(1.0f);
static const float selectedScale(1.1f);

CardNode* CardNode::create(bool canShake)
{
    CardNode *ret = new (nothrow) CardNode();
    if (ret && ret->init(canShake)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CardNode::CardNode()
:_observer(nullptr)
,_cardWidget(nullptr)
,_iconSprite(nullptr)
,_qualitySprite(nullptr)
,_resourceNode(nullptr)
,_shiningSprite(nullptr)
,_coldDownProgress(nullptr)
,_card(nullptr)
,_touchInvalid(false)
,_selected(false)
,_isShaking(false)
,_basePoint(Point::ZERO)
{
    
}

CardNode::~CardNode()
{
    removeAllChildren();
}

bool CardNode::init(bool canShake)
{
    if (Node::init())
    {
        _canShake = canShake;
        
        static const string csbFile("UI_Card.csb");
        Node *mainNode = CocosUtils::playCSBAnimation(csbFile, false, 0, nullptr);
        addChild(mainNode);
        
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
                        _resourceNode = readdResourceNode(child, ::ResourceType::Gold, 0);
                    }
                        break;
                    default:
                        break;
                }
            }
        }
        
        const Size size(_cardWidget->getContentSize());
        const Point mid(size.width / 2, size.height / 2);
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        mainNode->setPosition(mid);
        
        _cardWidget->addTouchEventListener([=](Ref *pSender, Widget::TouchEventType type) {
            Widget* button = dynamic_cast<Widget*>(pSender);
            if (type == Widget::TouchEventType::BEGAN) {
                _touchInvalid = false;
                
                _cardWidget->setScale(_selected ? 1.05f : 0.95f);
                
                if(_observer) {
                    _observer->onCardNodeTouchedBegan(this);
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
                
                _cardWidget->setScale(_selected ? selectedScale : normalScale);
                
                if(_observer) {
                    _observer->onCardNodeTouchedEnded(this, !_touchInvalid);
                }
            } else if (type == Widget::TouchEventType::CANCELED) {
                _cardWidget->setScale(_selected ? selectedScale : normalScale);
            }
        });
        
        // spell CD
        {
            _coldDownProgress = ProgressTimer::create(Sprite::create("GameImages/test/ui_iconzhezhao_white.png"));
            _coldDownProgress->setType(ProgressTimer::Type::RADIAL);
            _coldDownProgress->setReverseDirection(true);
            _coldDownProgress->setMidpoint(Point::ANCHOR_MIDDLE);
            _coldDownProgress->setPosition(mid);
            _cardWidget->addChild(_coldDownProgress);
        }
        
        // spell activated sprite
        {
            _shiningSprite = Sprite::create("GameImages/test/ui_xuanzhong.png");
            _shiningSprite->setVisible(false);
            _shiningSprite->setPosition(mid);
            _cardWidget->addChild(_shiningSprite);
        }
        
        return true;
    }
    
    return false;
}

void CardNode::registerObserver(CardNodeObserver *observer)
{
    _observer = observer;
}

void CardNode::setPosition(const Vec2& position)
{
    setPosition(position.x, position.y);
}

void CardNode::setPosition(float x, float y)
{
    Node::setPosition(x, y);
    
    if (_isShaking) {
        // TODO:
    } else {
        _basePoint.x = x;
        _basePoint.y = y;
        resetPosition();
    }
}

void CardNode::setOpacity(GLubyte opacity)
{
    Node::setOpacity(opacity);
    
    if (_iconSprite) {
        _iconSprite->setOpacity(opacity);
    }
    
    if (_qualitySprite) {
        _qualitySprite->setOpacity(opacity);
    }
    
    if (_shiningSprite) {
        _shiningSprite->setOpacity(opacity);
    }
    
    if (_resourceNode) {
        _resourceNode->setOpacity(opacity);
    }
    
    for (auto iter = begin(_starSprites); iter != end(_starSprites); ++iter) {
        (*iter).second->setOpacity(opacity);
    }
}

void CardNode::update(const std::string& name)
{
    _cardName = name;
    
    updateIcon(true);
    
    if (_resourceNode) {
        _resourceNode->removeFromParent();
        _resourceNode = nullptr;
    }
    
    for (auto iter = begin(_starSprites); iter != end(_starSprites); ++iter) {
        iter->second->getParent()->getParent()->setVisible(false);
        break;
    }
}

void CardNode::updateCD(float percentage)
{
    bool disabled(percentage > 0);
    updateIcon(!disabled);
    
    if (_coldDownProgress) {
        _coldDownProgress->setVisible(disabled);
        if (disabled) {
            _coldDownProgress->setPercentage(100.0f * percentage);
        }
    }
}

void CardNode::update(const HMMCard* card, float resource)
{
    _card = card;
    
    if (card) {
        const HMMCardType* ct = card->getCardType();
        if (ct) {
            const string& name = ct->getName();
            
            int cost(0);
            static const string& resourceName(RESOURCE_NAME);
            const auto& costs = ct->getCost();
            if (costs.find(resourceName) != costs.end()) {
                cost = costs.at(resourceName) / GameConstants::MICRORES_PER_RES;
            }
            
            // TODO:
            const UnitType* ut = nullptr;
            if (ut) {
                update(name, ut->getRarity(), cost, resource);
            } else {
                update(name, 0, cost, resource);
            }
        }
    }
}

void CardNode::update(const string& name, int rarity, int cost, float resource)
{
    _cardName = name;
    
    // update mutable data
    updateIcon(resource >= cost);
    
    if (_qualitySprite) {
        _qualitySprite->setTexture(StringUtils::format("GameImages/test/ui_kuang_%d.png", rarity + 1));
    }
    
    // !!!if we didn't re-add the resource nodes, the animation would be stopped(It may caused by the table's refreshing)
    _resourceNode = readdResourceNode(_resourceNode, ::ResourceType::Gold, cost);
    checkResource(resource);
    
    for (auto iter = begin(_starSprites); iter != end(_starSprites); ++iter) {
        iter->second->getParent()->getParent()->setVisible(true);
        break;
    }
}

void CardNode::setSelected(bool selected)
{
    // 1. check if need shake
    const bool needShake(selected || _selected);
    
    // 2. save
    _selected = selected;
    
    if (_shiningSprite) {
        _shiningSprite->setVisible(selected);
    }
    
    _cardWidget->setScale(selected ? selectedScale : normalScale);
    
    // Follow this steps:
    // 1. set position
    resetPosition();
    
    // 2. play animation
    if (needShake) {
        shake();
    } else {
        stopShake();
    }
}

void CardNode::checkResource(float count)
{
    if (_resourceNode) {
        // if it is used
        if (getOpacity() < 255) {
            count = BATTLE_RESOURCE_MAX_COUNT;
        }
        
        _resourceNode->check(count);
        
        const int cost = _resourceNode->getCount();
        bool disabled(cost > count);
        updateIcon(!disabled);
        
        if (_coldDownProgress) {
            _coldDownProgress->setVisible(disabled);
            if (disabled) {
                _coldDownProgress->setPercentage(100.0f * (1.0f - count / cost));
            }
        }
    }
}

const HMMCard* CardNode::getCard() const
{
    return _card;
}

const string& CardNode::getCardName() const
{
    return _cardName;
}

string CardNode::getIconFile(const string& name, bool enable) const
{
    const CardConfigData* configData = DataManager::getInstance()->getCardConfigData(name);
    if (configData) {
        if (enable) {
            return configData->getIcon();
        } else {
            return configData->getDisabledIcon();
        }
    }
    
    return "";
}

BattleSmallResourceNode* CardNode::readdResourceNode(Node* currentNode, ::ResourceType type, int count)
{
    if (currentNode) {
        auto node = BattleSmallResourceNode::create(type, count);
        node->setPosition(currentNode->getPosition());
        node->setLocalZOrder(topZOrder);
        currentNode->getParent()->addChild(node);
        currentNode->removeFromParent();
        return node;
    }
    
    return nullptr;
}

void CardNode::updateIcon(bool colorful)
{
    const string& iconFile = getIconFile(_cardName, colorful);
    const bool show(_iconSprite && iconFile.length() > 0);
    if (_iconSprite) {
        _iconSprite->setVisible(show);
        if (show) {
            _iconSprite->setTexture(iconFile);
        }
    }
}

void CardNode::resetPosition()
{
    if (_basePoint.x != 0) {
        const Point& point = getPosition();
        const Point& pos = _basePoint + Point(0, _selected ? 10.0f : 0.0f);
        if (point != pos) {
            Node::setPosition(pos.x, pos.y);
        }
    }
}

#pragma mark shake
static float shake_action_tag = 2016;
void CardNode::shake()
{
    if (_canShake && !_isShaking) {
        stopShake();
        
        static float shake_duration = 0.2f;
        static float shake_strength = 1.0f;
        
        _isShaking = true;
        auto action = (Sequence::create(CCShake::actionWithDuration(shake_duration, shake_strength, getPosition()), CallFunc::create([=] {
            _isShaking = false;
            resetPosition();
        }), nullptr));
        
        action->setTag(shake_action_tag);
        runAction(action);
    }
}

void CardNode::stopShake()
{
    if (_canShake) {
        if (_isShaking) {
            _isShaking = false;
        }
        
        if (getActionByTag(shake_action_tag)) {
            stopActionByTag(shake_action_tag);
        }
    }
}
