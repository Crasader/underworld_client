//
//  MapUIUnitNode.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MapUIUnitNode.h"
#include "CocosUtils.h"
#include "Camp.h"
#include "ResourceButton.h"
#include "DataManager.h"
#include "URConfigData.h"
#include "SoundManager.h"

using namespace std;
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
,_shadow(nullptr)
,_iconButton(nullptr)
,_iconBasePosition(Point::ZERO)
,_mask(nullptr)
,_countLabel(nullptr)
,_camp(nullptr)
,_idx(CC_INVALID_INDEX)
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
#if false
        const string CsbFile = "facebook_UI.csb";
        Node *mainNode = CSLoader::createNode(CsbFile);
        addChild(mainNode);
        timeline::ActionTimeline *action = CSLoader::createTimeline(CsbFile);
        mainNode->runAction(action);
        action->gotoFrameAndPlay(0, false);
        Widget* root = dynamic_cast<Widget *>(mainNode->getChildByTag(501));
        root->setSwallowTouches(false);
        const Vector<Node*>& children = root->getChildren();
        for (int i = 0; i < children.size(); ++i)
        {
            Node* child = children.at(i);
            if (child) {
                const int tag = child->getTag();
                const Point& position = child->getPosition();
                Node *parent = child->getParent();
                switch (tag) {
                        
                }
            }
        }
#else
        _shadow = Sprite::create("GameImages/test/ui_iconyingzi.png");
        addChild(_shadow, bottomZOrder);
        
        const string& iconFile = getIconFile(camp, true);
        _iconButton = Button::create(iconFile, iconFile);
        _iconButton->setPressedActionEnabled(false);
        _iconButton->setSwallowTouches(false);
        addChild(_iconButton);
        _iconButton->addTouchEventListener([=](Ref *pSender, Widget::TouchEventType type) {
            Widget* button = dynamic_cast<Widget*>(pSender);
            if (type == Widget::TouchEventType::BEGAN) {
                addTouchedAction(true, true);
                
                _touchInvalid = false;
                if(_observer) {
                    _observer->onMapUIUnitNodeTouchedBegan(this);
                }
            } else if (type == Widget::TouchEventType::MOVED) {
                if (!_touchInvalid && button->getTouchMovePosition().distance(button->getTouchBeganPosition()) > TOUCH_CANCEL_BY_MOVING_DISTANCE) {
                    _touchInvalid = true;
                }
            } else if (type == Widget::TouchEventType::ENDED) {
                addTouchedAction(false, true);
                
                if (!_touchInvalid) {
                    SoundManager::getInstance()->playButtonSound();
                    if(_observer) {
                        _observer->onMapUIUnitNodeTouchedEnded(this);
                    }
                }
            } else {
                addTouchedAction(false, true);
                if(_observer) {
                    _observer->onMapUIUnitNodeTouchedCanceled(this);
                }
            }
        });
        
        _resourcesMask = Sprite::create("GameImages/test/ui_black_15.png");
        addChild(_resourcesMask);
        
        _countLabel = CocosUtils::createLabel("0", DEFAULT_FONT_SIZE);
        addChild(_countLabel);
        
        const Size& shadowSize = _shadow->getContentSize();
        const Size& iconSize = _iconButton->getContentSize();
        static const float offsetY(2.0f);
        static const float shadowOffsetY(5.0f);
        const Size size(MAX(shadowSize.width, iconSize.width), shadowSize.height / 2 + MAX(shadowSize.height / 2, iconSize.height / 2 + shadowOffsetY));
        setContentSize(size);
        
        const float x = size.width / 2;
        _shadow->setPosition(x, shadowSize.height / 2);
        _iconBasePosition = _shadow->getPosition() + Point(0, shadowOffsetY);
        _iconButton->setPosition(_iconBasePosition);
        
        _mask = Sprite::create("GameImages/test/ui_iconzhezhao.png");
        _mask->setPosition(Point(iconSize.width / 2, iconSize.height / 2));
        _mask->setVisible(false);
        _iconButton->addChild(_mask, topZOrder);
        
        {
            const float height = _countLabel->getContentSize().height;
            _countLabel->setPosition(Point(x, size.height - height / 2 - offsetY));
        }
        
        {
            const float height = _resourcesMask->getContentSize().height;
            _resourcesMask->setPosition(Point(x, height / 2));
            
            createResourceButton(kResourceType_Gold, _resourcesMask);
            createResourceButton(kResourceType_Wood, _resourcesMask);
        }
#endif
        
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
        const string& iconFile = getIconFile(_camp, enable);
        if (iconFile.length() > 0) {
            _iconButton->loadTextures(iconFile, iconFile);
        }
        
        const bool show = !isHero(_camp);
        _countLabel->setVisible(show);
        if (show) {
            _countLabel->setString(StringUtils::format("%d/%d", production, maxProduction));
        }
        
        const map<string, int>& costs = _camp->getCosts();
        ResourceButton* woodButton = _resourceButtons.at(kResourceType_Wood);
        woodButton->setVisible(costs.find(RES_NAME_WOOD) != costs.end());
        if (woodButton->isVisible()) {
            int count = costs.at(RES_NAME_WOOD);
            woodButton->setCount(count);
            woodButton->setEnabled(wood >= count);
        }
        
        ResourceButton* goldButton = _resourceButtons.at(kResourceType_Gold);
        goldButton->setVisible(costs.find(RES_NAME_GOLD) != costs.end());
        if (goldButton->isVisible()) {
            int count = costs.at(RES_NAME_GOLD);
            goldButton->setCount(count);
            goldButton->setEnabled(gold >= count);
        }
        
        const float resourceMaskWidth = _resourcesMask->getContentSize().width;
        if (woodButton->isVisible() && goldButton->isVisible()) {
            ResourceButton* woodButton = _resourceButtons.at(kResourceType_Wood);
            ResourceButton* goldButton = _resourceButtons.at(kResourceType_Gold);
            const float width1 = woodButton->getContentSize().width;
            const float width2 = goldButton->getContentSize().width;
            const float totalWidth = width1 + width2;
            const float offsetX = (resourceMaskWidth - totalWidth) / 2;
            woodButton->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
            goldButton->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
            woodButton->setPosition(offsetX, 0);
            goldButton->setPosition(offsetX + width1, 0);
            
        } else {
            ResourceButton* button = woodButton->isVisible() ? woodButton : goldButton;
            button->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
            button->setPosition(resourceMaskWidth / 2, 0);
        }
    }
}

void MapUIUnitNode::setSelected(bool selected)
{
#if false
    if (_iconButton) {
        static const int selectedTag(100);
        Node *selectedSprite = _iconButton->getChildByTag(selectedTag);
        if (selected) {
            if (!selectedSprite) {
                Sprite *s = Sprite::create("GameImages/test/ui_xuanzhong.png");
                s->setTag(selectedTag);
                const Size& size = _iconButton->getContentSize();
                s->setPosition(Point(size.width / 2, size.height / 2));
                _iconButton->addChild(s, topZOrder);
            }
        } else {
            if (selectedSprite) {
                _iconButton->removeChild(selectedSprite);
            }
        }
    }
#endif
}

void MapUIUnitNode::setTouched(bool touched)
{
    addTouchedAction(touched, false);
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
    const string& unitName = camp ? camp->getUnitSetting().getUnitTypeName() : "";
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
    const UnitType* unitType = camp->getUnitType();
    if (kUnitClass_Hero == unitType->getUnitClass()) {
        return true;
    }
    
    return false;
}

void MapUIUnitNode::createResourceButton(::ResourceType type, Node* parent)
{
    ResourceButton* button = ResourceButton::create(false, false, true, type, 0, nullptr);
    button->setAnchorPoint(Point::ANCHOR_MIDDLE);
    parent->addChild(button);
    _resourceButtons.insert(make_pair(type, button));
}

void MapUIUnitNode::addTouchedAction(bool touched, bool animated)
{
    if (_iconButton) {
        _iconButton->stopAllActions();
        _shadow->setVisible(!touched);
        _mask->setVisible(touched);
        
        const Point destinationPos = _iconBasePosition + (touched ? iconTouchOffset : Point::ZERO);
        if (_iconButton->getPosition() != destinationPos) {
            if (animated) {
                static float duration(0.2f);
                _iconButton->runAction(Sequence::create(MoveTo::create(duration, destinationPos), CallFunc::create([this] {
                }), nullptr));
            } else {
                _iconButton->setPosition(destinationPos);
            }
        }
    }
}
