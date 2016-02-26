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

BattleDeckTestNode* BattleDeckTestNode::create(const string& name, const string& renderKey, bool isHero)
{
    BattleDeckTestNode *ret = new (nothrow) BattleDeckTestNode();
    if (ret && ret->init(name, renderKey, isHero)) {
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
,_goldSprite(nullptr)
,_woodSprite(nullptr)
,_countLabel(nullptr)
,_goldLabel(nullptr)
,_woodLabel(nullptr)

,_shiningSprite(nullptr)
,_isHero(false)
,_touchInvalid(false)
,_isIconTouched(false)
{
    
}

BattleDeckTestNode::~BattleDeckTestNode()
{
    removeAllChildren();
}

bool BattleDeckTestNode::init(const string& name, const string& renderKey, bool isHero)
{
    if (Node::init())
    {
        _unitName = name;
        _renderKey = renderKey;
        _isHero = isHero;
        
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
                        const string& iconFile = getIconFile(renderKey, true);
                        _iconSprite = Sprite::create(iconFile);
                        child->addChild(_iconSprite);
                    }
                        break;
                    case 51: {
                        Node* node = child->getChildByTag(52);
                        if (node) {
                            _countLabel = CocosUtils::createLabel("0", DEFAULT_FONT_SIZE, DEFAULT_NUMBER_FONT);
                            node->addChild(_countLabel);
                            child->setVisible(!isHero);
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
        
        const Size size(_cardWidget->getContentSize());
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        mainNode->setPosition(Point(size.width / 2, size.height / 2));
        
        _cardWidget->setSwallowTouches(false);
        _cardWidget->addTouchEventListener([=](Ref *pSender, Widget::TouchEventType type) {
            Widget* button = dynamic_cast<Widget*>(pSender);
            if (type == Widget::TouchEventType::BEGAN) {
                _isIconTouched = true;
                addTouchedAction(true, true);
                
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
                _isIconTouched = false;
                addTouchedAction(false, true);
                
                if (!_touchInvalid) {
                    SoundManager::getInstance()->playButtonSound();
                }
                
                if(_observer) {
                    _observer->onBattleDeckTestNodeTouchedEnded(this, !_touchInvalid);
                }
            } else {
                _isIconTouched = false;
                addTouchedAction(false, true);
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
        
        return true;
    }
    
    return false;
}

void BattleDeckTestNode::registerObserver(BattleDeckTestNodeObserver *observer)
{
    _observer = observer;
}

void BattleDeckTestNode::reuse(const string& name, const string& renderKey)
{
    _unitName = name;
    _renderKey = renderKey;
    
    // update mutable data
    update(true);
}

void BattleDeckTestNode::update(bool reuse)
{
    const string& iconFile = getIconFile(_renderKey, true);
    if (iconFile.length() > 0) {
        _iconSprite->setTexture(iconFile);
    }
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

void BattleDeckTestNode::setTouched(bool touched, bool isGameOver)
{
    addTouchedAction(isGameOver ? _isIconTouched : touched, false);
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

bool BattleDeckTestNode::isHero() const
{
    return _isHero;
}

void BattleDeckTestNode::addTouchedAction(bool touched, bool animated)
{
    if (_cardWidget) {
        _cardWidget->stopAllActions();
    }
}
