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
        static float offsetX = 20.0f;
        if (_isScrollToLeft) {
            _button->setPosition(Point(position.x - offsetX, position.y + s1.height / 2));
        } else {
            _button->setPosition(Point(position.x + s1.width + offsetX, position.y + s1.height / 2));
        }
        _button->addClickEventListener([this](Ref*) {
            setFold(!_isFold, true);
        });
        _button->setVisible(false);
        addChild(_button);
        
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

void CampInfoNode::insert(const vector<pair<const Camp*, const UnitBase*>>& units)
{
    if (_displayIconNode) {
        _displayIconNode->setVisible(true);
        _displayIconNode->insert(units);
        if (!_unitInfoNode) {
            onDisplayIconNodeTouchedEnded(units.at(0).second);
        }
    }
    
    if (_button) {
        _button->setVisible(true);
    }
}

void CampInfoNode::update()
{
    if (_displayIconNode) {
        _displayIconNode->update();
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
        addChild(_unitInfoNode);
    }
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
    _isFold = fold;
    const string& file = getButtonFile(fold);
    _button->loadTextures(file, file);
    
    Point destinationPos(_basePosition);
    const Size& s1 = _displayIconNode->getContentSize();
    const Size& s2 = _unitInfoNode->getContentSize();
    float offset = MAX(s1.width, s2.width);
    if (_isScrollToLeft) {
        if (fold) {
            destinationPos = _basePosition + Point(offset, 0);
        }
    } else {
        if (fold) {
            destinationPos = _basePosition - Point(offset, 0);
        }
    }
    
    if (animated) {
        runAction(Sequence::create(MoveTo::create(0.5f, destinationPos), CallFunc::create([=] {
            
        }), nullptr));
    } else {
        setPosition(destinationPos);
    }
}
