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
#include "Deck.h"
#include "SoundManager.h"

using namespace std;
using namespace cocostudio;
using namespace UnderWorld::Core;

static const int topZOrder(1);
static const Point iconTouchOffset(0, -6.0f);

CardNode* CardNode::create()
{
    CardNode *ret = new (nothrow) CardNode();
    if (ret && ret->init()) {
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
{
    
}

CardNode::~CardNode()
{
    removeAllChildren();
}

bool CardNode::init()
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
                        _resourceNode = readdResourceNode(child, kResourceType_Gold, 0);
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
                
                if(_observer) {
                    _observer->onCardNodeTouchedEnded(this, !_touchInvalid);
                }
            }
        });
        
        // spell CD
        {
            _coldDownProgress = ProgressTimer::create(Sprite::create("GameImages/test/ui_iconzhezhao_white.png"));
            _coldDownProgress->setType(ProgressTimer::Type::RADIAL);
            _coldDownProgress->setReverseDirection(true);
            _coldDownProgress->setMidpoint(Point::ANCHOR_MIDDLE);
            _coldDownProgress->setPosition(mid);
            _cardWidget->addChild(_coldDownProgress, topZOrder);
        }
        
        // spell activated sprite
        {
            _shiningSprite = Sprite::create("GameImages/test/ui_xuanzhong.png");
            _shiningSprite->setVisible(false);
            _shiningSprite->setPosition(mid);
            _cardWidget->addChild(_shiningSprite, topZOrder);
        }
        
        return true;
    }
    
    return false;
}

void CardNode::registerObserver(CardNodeObserver *observer)
{
    _observer = observer;
}

void CardNode::update(const Card* card, float resource)
{
    _card = card;
    
    if (card) {
        int cost(0);
        static const string resourceName = RES_NAME_WOOD;
        const auto& costs = _card->getCardType()->getCost();
        if (costs.find(resourceName) != costs.end()) {
            cost = costs.at(resourceName);
        }
        
        const UnitType* ut = card->getUnitType();
        if (ut) {
            update(ut->getName(), ut->getRarity(), cost, resource);
        } else {
            const SpellType* st = card->getSpellType();
            if (st) {
                update(st->getSpellName(), 0, cost, resource);
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
    _resourceNode = readdResourceNode(_resourceNode, kResourceType_Gold, cost);
    checkResource(resource);
}

void CardNode::setSelected(bool selected)
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

void CardNode::checkResource(float count)
{
    if (_resourceNode) {
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

const Card* CardNode::getCard() const
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
    string iconFile;
    static const string defaultFile("GameImages/icons/unit/big/normal/icon_w_langdun.png");
    if (enable) {
        iconFile = configData ? configData->getIcon() : defaultFile;
    } else {
        iconFile = configData ? configData->getDisabledIcon() : defaultFile;
    }
    
    return iconFile;
}

BattleSmallResourceNode* CardNode::readdResourceNode(Node* currentNode, ::ResourceType type, int count)
{
    if (currentNode) {
        auto node = BattleSmallResourceNode::create(type, count);
        node->setPosition(currentNode->getPosition());
        currentNode->getParent()->addChild(node);
        currentNode->removeFromParent();
        return node;
    }
    
    return nullptr;
}

void CardNode::updateIcon(bool colorful)
{
    const string& iconFile = getIconFile(_cardName, colorful);
    if (_iconSprite && iconFile.length() > 0) {
        _iconSprite->setTexture(iconFile);
    }
}
