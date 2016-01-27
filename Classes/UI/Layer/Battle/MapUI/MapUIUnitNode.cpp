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
,_shadow(nullptr)
,_iconButton(nullptr)
,_iconBasePosition(Point::ZERO)
,_mask(nullptr)
,_resourcesMask(nullptr)
,_shiningSprite(nullptr)
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
        // background shadow
        {
            _shadow = Sprite::create("GameImages/test/ui_iconyingzi.png");
            addChild(_shadow, bottomZOrder);
        }
        
        const string& iconFile = getIconFile(camp, true);
        _iconButton = Button::create(iconFile, iconFile);
        _iconButton->setPressedActionEnabled(false);
        _iconButton->setSwallowTouches(false);
        addChild(_iconButton);
        _iconButton->addTouchEventListener([=](Ref *pSender, Widget::TouchEventType type) {
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
        
        // count label
        {
            _countLabel = CocosUtils::createLabel(" ", DEFAULT_FONT_SIZE);
            addChild(_countLabel);
        }
        
        const Size& shadowSize = _shadow->getContentSize();
        const Size& iconSize = _iconButton->getContentSize();
        const float countLabelheight = _countLabel->getContentSize().height;
        static const float shadowOffsetY(5.0f);
        const Size size(MAX(shadowSize.width, iconSize.width), shadowSize.height / 2 + MAX(shadowSize.height / 2, iconSize.height / 2 + shadowOffsetY) + countLabelheight);
        setContentSize(size);
        
        const float x = size.width / 2;
        _shadow->setPosition(x, shadowSize.height / 2);
        _iconBasePosition = _shadow->getPosition() + Point(0, shadowOffsetY);
        _iconButton->setPosition(_iconBasePosition);
        _countLabel->setPosition(_iconBasePosition + Point(0, (iconSize.height + countLabelheight) / 2));
        
        // mask
        {
            _mask = Sprite::create("GameImages/test/ui_iconzhezhao.png");
            _mask->setPosition(Point(iconSize.width / 2, iconSize.height / 2));
            _mask->setVisible(false);
            _iconButton->addChild(_mask, topZOrder);
        }
        
        // resources
        {
            _resourcesMask = Sprite::create("GameImages/test/ui_black_15.png");
            const Size& size = _resourcesMask->getContentSize();
            _resourcesMask->setPosition(Point(size.width / 2, size.height / 2));
            _iconButton->addChild(_resourcesMask);
            
            createResourceButton(kResourceType_Gold, _resourcesMask);
            createResourceButton(kResourceType_Wood, _resourcesMask);
        }
        
        // spell CD
        {
            _spellColdDown = ProgressTimer::create(Sprite::create("GameImages/test/ui_iconzhezhao.png"));
            _spellColdDown->setType(ProgressTimer::Type::RADIAL);
            _spellColdDown->setReverseDirection(true);
            _spellColdDown->setMidpoint(Vec2(0.5f, 0.5f));
            _iconButton->addChild(_spellColdDown, topZOrder);
            _spellColdDown->setPosition(Point(iconSize.width / 2, iconSize.height / 2));
        }
        
        // spell activated sprite
        {
            _shiningSprite = Sprite::create("GameImages/test/ui_xuanzhong.png");
            _shiningSprite->setPosition(Point(iconSize.width / 2, iconSize.height / 2));
            _shiningSprite->setVisible(false);
            _iconButton->addChild(_shiningSprite, topZOrder);
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
        const bool hero = isHero(_camp);
        const Unit* heroUnit = _camp->getHero();
        
        if (!hero) {
            _countLabel->setString(StringUtils::format("%d/%d", production, maxProduction));
        }
        
        bool colorful(true);
        const map<string, int>& costs = _camp->getCosts();
        ResourceButton* woodButton = _resourceButtons.at(kResourceType_Wood);
        woodButton->setVisible(costs.find(RES_NAME_WOOD) != costs.end() && enable);
        if (woodButton->isVisible()) {
            int count = costs.at(RES_NAME_WOOD);
            woodButton->setCount(count);
            if (wood < count) {
                colorful = false;
            }
//            woodButton->setEnabled(wood >= count);
        }
        
        ResourceButton* goldButton = _resourceButtons.at(kResourceType_Gold);
        goldButton->setVisible(costs.find(RES_NAME_GOLD) != costs.end() && enable);
        if (goldButton->isVisible()) {
            int count = costs.at(RES_NAME_GOLD);
            goldButton->setCount(count);
            if (gold < count) {
                colorful = false;
            }
//            goldButton->setEnabled(gold >= count);
        }
        
        const float resourceMaskWidth = _resourcesMask->getContentSize().width;
        _resourcesMask->setVisible(woodButton->isVisible() || goldButton->isVisible());
        if (woodButton->isVisible() && goldButton->isVisible()) {
            ResourceButton* woodButton = _resourceButtons.at(kResourceType_Wood);
            ResourceButton* goldButton = _resourceButtons.at(kResourceType_Gold);
            const float width1 = woodButton->getContentSize().width;
            const float width2 = goldButton->getContentSize().width;
            const float totalWidth = width1 + width2;
            const float offsetX = (resourceMaskWidth - totalWidth) / 2;
            static const float gap(10.0f);
            woodButton->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
            goldButton->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
            woodButton->setPosition(offsetX, 0);
            goldButton->setPosition(offsetX + width1 + gap, 0);
            
        } else {
            ResourceButton* button = woodButton->isVisible() ? woodButton : goldButton;
            button->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
            button->setPosition(resourceMaskWidth / 2, 0);
        }
        
        if (hero && colorful) {
            if (heroUnit) {
                if (!heroUnit->isAlive()) {
                    colorful = false;
                }
            } else {
                if (!_resourcesMask->isVisible()) {
                    colorful = false;
                }
            }
            
            if (!colorful) {
                _countLabel->setString("召唤中");
            } else {
                _countLabel->setString(" ");
            }
        } else {
            _countLabel->setString(" ");
        }
        
        const string& iconFile = getIconFile(_camp, colorful);
        if (iconFile.length() > 0) {
            _iconButton->loadTextures(iconFile, iconFile);
        }
        
        int spellCD(10);
        if (heroUnit && heroUnit->getSpellCount() > 0) {
            spellCD = heroUnit->getSpellByIndex(0)->getCDProgress();
        }
        
        if (!heroUnit || spellCD == 0) {
            _spellColdDown->setVisible(false);
        } else {
            _spellColdDown->setVisible(true);
            if (colorful) {
                _spellColdDown->setSprite(Sprite::create("GameImages/test/ui_iconzhezhao.png"));
            } else {
                _spellColdDown->setSprite(Sprite::create("GameImages/test/ui_iconzhezhao_white.png"));
            }
            
            if (heroUnit->getSpellCount() > 0) {
                _spellColdDown->setPercentage(((float)spellCD) * 100.f / heroUnit->getSpellByIndex(0)->getTotalCDFrames());
            }
        }
        
        // add effect
        if (_iconButton) {
#if true
            if (heroUnit && heroUnit->isAlive() && spellCD == 0) {
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
#if false
    if (_shiningSprite) {
        _shiningSprite->setVisible(selected);
    }
#endif
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
    Color4B color = (type == kResourceType_Gold) ? GOLD_LABEL_COLOR : WOOD_LABEL_COLOR;
    ResourceButton* button = ResourceButton::create(false, false, true, kResourceType_MAX, 0, color, nullptr);
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
