//
//  CardNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/2/22.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "CardNode.h"
#include "CocosUtils.h"
#include "BattleSmallResourceNode.h"
#include "DataManager.h"
#include "CardConfigData.h"
#include "GameModeHMM.h"
#include "CCShake.h"
#include "SoundManager.h"

using namespace std;

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
,_cardId(0)
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
        Node *mainNode = CocosUtils::playAnimation(csbFile, 0, false);
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
        
        CocosUtils::fixWidgetTouchEvent(_cardWidget, _touchInvalid, [this](Ref*, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::BEGAN) {
                _cardWidget->setScale(_selected ? 1.05f : 0.95f);
                
                if(_observer) {
                    _observer->onCardNodeTouchedBegan(this);
                }
            } else if (type == Widget::TouchEventType::ENDED) {
                _cardWidget->setScale(_selected ? selectedScale : normalScale);
                
                if(_observer) {
                    _observer->onCardNodeTouchedEnded(this, !_touchInvalid);
                }
            } else if (type == Widget::TouchEventType::CANCELED) {
                _cardWidget->setScale(_selected ? selectedScale : normalScale);
            }
        }, [this](Ref*) {
            SoundManager::getInstance()->playButtonSound();
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

void CardNode::updateWithoutInfo(int idx)
{
    _cardId = idx;
    
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
#if false
    updateIcon(!disabled);
#endif
    
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
        auto ct = card->getCardType();
        if (ct) {
            update(ct->getId(), ct, resource);
        }
    }
}

void CardNode::update(int idx, float resource)
{
    _card = nullptr;
    auto ct = DataManager::getInstance()->getGameModeHMM()->findCardTypeById(idx);
    update(idx, ct, resource);
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
            count = DataManager::getInstance()->getBattleResourceMaxCount();
        }
        
        _resourceNode->check(count);
        
        const int cost = _resourceNode->getCount();
        bool disabled(cost > count);
        updateIcon(!disabled);
        
#if false
        if (_coldDownProgress) {
            _coldDownProgress->setVisible(disabled);
            if (disabled) {
                _coldDownProgress->setPercentage(100.0f * (1.0f - count / cost));
            }
        }
#endif
    }
}

const HMMCard* CardNode::getCard() const
{
    return _card;
}

int CardNode::getCardId() const
{
    return _cardId;
}

int CardNode::getCost() const
{
    if (_resourceNode) {
        return _resourceNode->getCount();
    }
    
    return 0;
}

#pragma mark - protected
void CardNode::update(int idx, const HMMCardType* ct, float resource)
{
    _cardId = idx;
    
    // rarity
    auto dm = DataManager::getInstance();
    int rarity(0);
    auto ut = dm->getTechTree()->findUnitTypeById(idx);
    if (ut) {
        rarity = ut->getRarity();
    }
    
    // resource type & cost
    ::ResourceType type(::ResourceType::Gold);
    int cost(0);
    if (ct) {
        static const map<string, ::ResourceType> resources = {
            {RES_NAME_GOLD, ::ResourceType::Gold},
            {RES_NAME_WOOD, ::ResourceType::Wood}};
        const auto& costs = ct->getCost();
        for (auto iter = begin(resources); iter != end(resources); ++iter) {
            const string& resourceName = iter->first;
            if (costs.find(resourceName) != end(costs)) {
                type = iter->second;
                cost = costs.at(resourceName) / UnderWorld::Core::GameConstants::MICRORES_PER_RES;
                break;
            }
        }
    }
    
    // update mutable data
    updateIcon(resource >= cost);
    
    if (_qualitySprite) {
        _qualitySprite->setTexture(StringUtils::format("GameImages/test/ui_kuang_%d.png", rarity + 1));
    }
    
    // !!!if we didn't re-add the resource nodes, the animation would be stopped(It may caused by the table's refreshing)
    _resourceNode = readdResourceNode(_resourceNode, type, cost);
    checkResource(resource);
    
    for (auto iter = begin(_starSprites); iter != end(_starSprites); ++iter) {
        iter->second->getParent()->getParent()->setVisible(true);
        break;
    }
}

string CardNode::getIconFile(int idx, bool enable) const
{
    auto configData = DataManager::getInstance()->getCardConfigData(idx);
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
    const string& iconFile = getIconFile(_cardId, colorful);
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
