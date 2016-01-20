//
//  MapUIUnitNode.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MapUIUnitNode.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "Camp.h"
#include "ResourceButton.h"
#include "DataManager.h"
#include "URConfigData.h"
#include "SoundManager.h"

using namespace std;
using namespace UnderWorld::Core;

MapUIUnitNode* MapUIUnitNode::create(const Camp* camp, ssize_t idx)
{
    MapUIUnitNode *ret = new (nothrow) MapUIUnitNode();
    if (ret && ret->init(camp, idx))
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
,_iconButton(nullptr)
,_resourceButton(nullptr)
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

bool MapUIUnitNode::init(const Camp* camp, ssize_t idx)
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
        const string& iconFile = getIconFile(camp);
        _iconButton = Button::create(iconFile, iconFile);
        _iconButton->setPressedActionEnabled(false);
        _iconButton->setSwallowTouches(false);
        addChild(_iconButton);
        _iconButton->addTouchEventListener([=](Ref *pSender, Widget::TouchEventType type) {
            Widget* button = dynamic_cast<Widget*>(pSender);
            if (type == Widget::TouchEventType::BEGAN) {
                _touchInvalid = false;
                if(_observer) {
                    _observer->onMapUIUnitNodeTouchedBegan(this);
                }
            } else if (type == Widget::TouchEventType::MOVED) {
                if (!_touchInvalid && button->getTouchMovePosition().distance(button->getTouchBeganPosition()) > TOUCH_CANCEL_BY_MOVING_DISTANCE) {
                    _touchInvalid = true;
                }
            } else if (type == Widget::TouchEventType::ENDED) {
                if (!_touchInvalid) {
                    SoundManager::getInstance()->playButtonSound();
                    if(_observer) {
                        _observer->onMapUIUnitNodeTouchedEnded(this);
                    }
                }
            }
        });
        
        if (camp) {
            const map<string, int>& costs = camp->getCosts();
            if (costs.find(RES_NAME_GOLD) != costs.end()) {
                _resourceButton = ResourceButton::create(false, kResourceType_Gold, costs.at(RES_NAME_GOLD), nullptr);
                addChild(_resourceButton);

            }
        } else {
            _resourceButton = ResourceButton::create(false, kResourceType_Gold, 100, nullptr);
            addChild(_resourceButton);
        }
        
        _countLabel = CocosUtils::createLabel("0", DEFAULT_FONT_SIZE);
        addChild(_countLabel);
        
        const float iconHeight = _iconButton->getContentSize().height;
        const float resourceHeight = _resourceButton->getContentSize().height;
        const float countHeight = _countLabel->getContentSize().height;
        static const float offsetY(2.0f);
#if true
        const Size size(_iconButton->getContentSize().width, iconHeight);
        setContentSize(size);
        
        const float x = size.width / 2;
        const float y = size.height / 2;
        _iconButton->setPosition(Point(x, y));
        _resourceButton->setAnchorPoint(Point::ANCHOR_MIDDLE);
        _resourceButton->setPosition(Point(x, resourceHeight / 2 + offsetY));
        _countLabel->setPosition(Point(x, size.height - countHeight / 2 - offsetY));
#else
        const Size size(_iconButton->getContentSize().width, iconHeight + resourceHeight + countHeight + offsetY * 4);
        setContentSize(size);
        
        const float x = size.width / 2;
        const float y = size.height / 2;
        _iconButton->setPosition(Point(x, y));
        _resourceButton->setAnchorPoint(Point::ANCHOR_MIDDLE);
        _resourceButton->setPosition(Point(x, y - (iconHeight + resourceHeight) / 2 - offsetY));
        _countLabel->setPosition(Point(x, y + (iconHeight + countHeight) / 2 + offsetY));
#endif
        
        if (camp) {
            reuse(camp, idx);
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

void MapUIUnitNode::reuse(const Camp* camp, ssize_t idx)
{
    _camp = camp;
    _idx = idx;
    
    const string& iconFile = getIconFile(camp);
    if (iconFile.length() > 0) {
        _iconButton->loadTextures(iconFile, iconFile);
    }
    
    // update mutable data
    update(true);
}

void MapUIUnitNode::update(bool reuse)
{
    if (_camp) {
        const bool show = !isHero(_camp);
        _countLabel->setVisible(show);
        if (show) {
            _countLabel->setString(StringUtils::format("%d/%d", _camp->getProduction(), _camp->getMaxProduction()));
        }
        
        const map<string, int> costs = _camp->getCosts();
        if (costs.find(RES_NAME_GOLD) != costs.end()) {
            _resourceButton->setCount(costs.at(RES_NAME_GOLD));
        }

    }
    
    if (!reuse) {
        if (_observer) {
            _observer->onMapUIUnitNodeUpdated(this);
        }
    }
}

void MapUIUnitNode::setSelected(bool selected)
{
    if (_iconButton) {
        Node *parent = _iconButton->getParent();
        if (parent) {
            static const int selectedTag(100);
            Node *selectedSprite = parent->getChildByTag(selectedTag);
            if (selected) {
                if (!selectedSprite) {
                    Sprite *s = Sprite::create("GameImages/test/ui_xuanzhong.png");
                    s->setTag(selectedTag);
                    s->setPosition(_iconButton->getPosition());
                    parent->addChild(s, 1);
                }
            } else {
                if (selectedSprite) {
                    parent->removeChild(selectedSprite);
                }
            }
        }
    }
}

const Camp* MapUIUnitNode::getCamp() const
{
    return _camp;
}

ssize_t MapUIUnitNode::getIdx() const
{
    return _idx;
}

string MapUIUnitNode::getIconFile(const Camp* camp) const
{
    const string& unitName = camp ? camp->getUnitSetting().getUnitTypeName() : "";
    const URConfigData* configData = DataManager::getInstance()->getURConfigData(unitName);
    const string& iconFile = configData ? configData->getIcon() : "GameImages/icons/unit/big/icon_w_langdun.png";
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