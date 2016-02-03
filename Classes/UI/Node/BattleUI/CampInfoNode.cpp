//
//  CampInfoNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "CampInfoNode.h"
#include "CocosGlobal.h"
#include "DisplayIconNode.h"
#include "UnitInfoNode.h"
#include "SoundManager.h"

using namespace std;
using namespace ui;
using namespace UnderWorld::Core;

static string rightFile("GameImages/public/button_sanjiao_2.png");
static string leftFile("GameImages/public/button_sanjiao_3.png");

CampInfoNode* CampInfoNode::create(bool scrollToLeft)
{
    CampInfoNode *ret = new (nothrow) CampInfoNode();
    if (ret && ret->init(scrollToLeft))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CampInfoNode::CampInfoNode()
:_observer(nullptr)
,_displayIconNode(nullptr)
,_unitInfoNode(nullptr)
,_button(nullptr)
,_isFold(false)
,_isMoving(false)
{
    
}

CampInfoNode::~CampInfoNode()
{
    removeAllChildren();
}

bool CampInfoNode::init(bool scrollToLeft)
{
    if (Node::init()) {
        _isScrollToLeft = scrollToLeft;
        
        _displayIconNode = DisplayIconNode::create(scrollToLeft);
        const Size& s1 = _displayIconNode->getContentSize();
        const Size& s2 = UnitInfoNode::create(nullptr)->getContentSize();
        const Size size(s2.width, s1.height + s2.height);
        setContentSize(size);
        
        Point position(Point::ZERO);
        if (scrollToLeft) {
            position = Point(size.width - s1.width, s2.height);
        } else {
            position = Point(0, s2.height);
        }
        _displayIconNode->setPosition(position);
        _displayIconNode->registerObserver(this);
        _displayIconNode->setVisible(false);
        addChild(_displayIconNode);
        
        const string& file = getButtonFile(_isFold);
        _button = Button::create(file, file);
        _button->addClickEventListener([this](Ref*) {
            setFold(!_isFold, true);
        });
        _button->setVisible(false);
        addChild(_button);
        
        resetButtonPosition();
        
        return true;
    }
    
    return false;
}

void CampInfoNode::setPosition(const Point& position)
{
    Node::setPosition(position);
    _basePosition = position;
}

void CampInfoNode::registerObserver(CampInfoNodeObserver *observer)
{
    _observer = observer;
}

#if ENABLE_CAMP_INFO
void CampInfoNode::insert(const vector<pair<const Camp*, const UnitBase*>>& units)
{
    if (_displayIconNode) {
        _displayIconNode->setVisible(true);
        _displayIconNode->insert(units);
    }
    
    if (_button) {
        _button->setVisible(true);
    }
}
#else
void CampInfoNode::insert(const std::vector<const UnderWorld::Core::UnitBase*>& units)
{
    if (_displayIconNode) {
        _displayIconNode->setVisible(true);
        _displayIconNode->insert(units);
    }
    
    if (_button) {
        _button->setVisible(true);
    }
}
#endif

void CampInfoNode::update()
{
    if (_displayIconNode) {
        _displayIconNode->update();
    }
}

void CampInfoNode::closeUnitInfoNode()
{
    if (_unitInfoNode) {
        _unitInfoNode->removeFromParent();
        _unitInfoNode = nullptr;
    }
}

Rect CampInfoNode::getIconsBoundingBox()
{
    if (_displayIconNode && _displayIconNode->isVisible()) {
        return _displayIconNode->getBoundingBox();
    }
    
    return Rect::ZERO;
}

#pragma mark - DisplayIconNodeObserver
void CampInfoNode::onDisplayIconNodeTouchedEnded(const UnitBase* unit)
{
    if (_unitInfoNode) {
        _unitInfoNode->update(unit);
    } else {
        _unitInfoNode = UnitInfoNode::create(unit);
        _unitInfoNode->registerObserver(this);
        addChild(_unitInfoNode);
    }
    
    if (_observer) {
        _observer->onCampInfoNodeClickedIcon(this, unit);
    }
}

void CampInfoNode::onDisplayIconNodeChangedContentSize(const Size& lastSize, const Size& newSize)
{
    setContentSize(newSize);
    
    if (_isFold) {
        const float offset = newSize.width - lastSize.width;
        const Point& lastPosition = getPosition();
        Node::setPositionX(lastPosition.x + (_isScrollToLeft ? offset : -offset));
    }
    
    resetButtonPosition();
}

#pragma mark - private
string CampInfoNode::getButtonFile(bool isFold)
{
    bool right(false);
    if (_isScrollToLeft) {
        if (!isFold) {
            right = true;
        }
    } else if (isFold) {
        right = true;
    }
    
    return right ? rightFile : leftFile;
}

void CampInfoNode::setFold(bool fold, bool animated)
{
    if (animated && _isMoving) {
        return;
    }
    
    _isFold = fold;
    const string& file = getButtonFile(fold);
    _button->loadTextures(file, file);
    
    Point destinationPos(_basePosition);
    const Size& size = _displayIconNode->getContentSize();
    float offset = size.width;
    if (fold) {
        destinationPos = _basePosition + Point(_isScrollToLeft ? offset : (-offset), 0);
    }
    
    if (animated) {
        _isMoving = true;
        runAction(Sequence::create(MoveTo::create(0.5f, destinationPos), CallFunc::create([=] {
            _isMoving = false;
        }), nullptr));
    } else {
        Node::setPosition(destinationPos);
    }
    
    closeUnitInfoNode();
}

void CampInfoNode::resetButtonPosition()
{
    if (_button) {
        static float offsetX = 20.0f;
        const Size& size = _displayIconNode->getContentSize();
        const Point& position = _displayIconNode->getPosition();
        const float posY = position.y + size.height / 2;
        if (_isScrollToLeft) {
            _button->setPosition(Point(position.x - offsetX, posY));
        } else {
            _button->setPosition(Point(position.x + size.width + offsetX, posY));
        }
    }
}
