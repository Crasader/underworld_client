//
//  BattleDeckTestNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/2/22.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "BattleDeckTestNode.h"
#include "cocostudio/CocoStudio.h"
#include "CocosUtils.h"
#include "BattleSmallResourceNode.h"
#include "DataManager.h"
#include "URConfigData.h"
#include "SoundManager.h"

using namespace std;
using namespace cocostudio;
using namespace UnderWorld::Core;

static const int topZOrder(1);
static const Point iconTouchOffset(0, -6.0f);

BattleDeckTestNode* BattleDeckTestNode::create(const string& name, const string& renderKey, bool isHero, int rarity)
{
    BattleDeckTestNode *ret = new (nothrow) BattleDeckTestNode();
    if (ret && ret->init(name, renderKey, isHero, rarity)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

BattleDeckTestNode::BattleDeckTestNode()
:_observer(nullptr)
,_cardWidget(nullptr)
,_addButton(nullptr)
,_iconSprite(nullptr)
,_qualitySprite(nullptr)
,_countNode(nullptr)
,_countLabel(nullptr)
,_goldNode(nullptr)
,_woodNode(nullptr)
,_shiningSprite(nullptr)
,_touchInvalid(false)
{
    
}

BattleDeckTestNode::~BattleDeckTestNode()
{
    removeAllChildren();
}

bool BattleDeckTestNode::init(const string& name, const string& renderKey, bool isHero, int rarity)
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
                    case 51: {
                        _countNode = child;
                        
                        Node* node = child->getChildByTag(52);
                        if (node) {
                            _countLabel = CocosUtils::createLabel("0", DEFAULT_FONT_SIZE, DEFAULT_NUMBER_FONT);
                            node->addChild(_countLabel);
                        } else {
                            assert(false);
                        }
                    }
                        break;
                    case 47: {
                        child->setLocalZOrder(topZOrder);
                        
                        Node* node = child->getChildByTag(48);
                        if (node) {
                            _goldNode = readdResourceNode(node, kResourceType_Gold, 0);
                        } else {
                            assert(false);
                        }
                    }
                        break;
                    case 49: {
                        child->setLocalZOrder(topZOrder);
                        
                        Node* node = child->getChildByTag(50);
                        if (node) {
                            _woodNode = readdResourceNode(node, kResourceType_Wood, 0);
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
        
        const Size size(_cardWidget->getContentSize());
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        mainNode->setPosition(Point(size.width / 2, size.height / 2));
        
        _cardWidget->setSwallowTouches(false);
        _cardWidget->addTouchEventListener([=](Ref *pSender, Widget::TouchEventType type) {
            Widget* button = dynamic_cast<Widget*>(pSender);
            if (type == Widget::TouchEventType::BEGAN) {
                _touchInvalid = false;
                if(_observer) {
                    _observer->onBattleDeckTestNodeTouchedBegan(this);
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
                    _observer->onBattleDeckTestNodeTouchedEnded(this, !_touchInvalid);
                }
            } else {
                if(_observer) {
                    _observer->onBattleDeckTestNodeTouchedCanceled(this);
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
        
        update(name, renderKey, isHero, rarity);
        
        return true;
    }
    
    return false;
}

void BattleDeckTestNode::registerObserver(BattleDeckTestNodeObserver *observer)
{
    _observer = observer;
}

void BattleDeckTestNode::update(const string& name, const string& renderKey, bool isHero, int rarity)
{
    _unitName = name;
    
    // update mutable data
    const string& iconFile = getIconFile(renderKey, true);
    if (iconFile.length() > 0) {
        _iconSprite->setTexture(iconFile);
    }
    
    if (_countNode) {
        _countNode->setVisible(!isHero);
    }
    
    if (_qualitySprite) {
        _qualitySprite->setTexture(StringUtils::format("GameImages/test/ui_kuang_%d.png", rarity + 1));
    }
    
    // !!!if we didn't re-add the resource nodes, the animation would be stopped(It may caused by the table's refreshing)
    _goldNode = readdResourceNode(_goldNode, kResourceType_Gold, 0);
    _woodNode = readdResourceNode(_woodNode, kResourceType_Wood, 0);
}

void BattleDeckTestNode::setSelected(bool selected)
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

const string& BattleDeckTestNode::getUnitName() const
{
    return _unitName;
}

string BattleDeckTestNode::getIconFile(const string& name, bool enable) const
{
    const URConfigData* configData = DataManager::getInstance()->getURConfigData(name);
    string iconFile;
    static const string defaultFile("GameImages/icons/unit/big/normal/icon_w_langdun.png");
    if (enable) {
        iconFile = configData ? configData->getIcon() : defaultFile;
    } else {
        iconFile = configData ? configData->getDisabledIcon() : defaultFile;
    }
    
    return iconFile;
}

BattleSmallResourceNode* BattleDeckTestNode::readdResourceNode(Node* currentNode, ::ResourceType type, int count)
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
