//
//  ResourceButton.cpp
//  ZombiePop
//
//  Created by Andy on 15/7/14.
//
//

#include "ResourceButton.h"
#include "cocostudio/CocoStudio.h"
#include "CocosUtils.h"
#include "SoundManager.h"

using namespace std;
using namespace cocostudio;

ResourceButton::ResourceButton()
:_animated(false)
,_needResize(false)
,_enabled(true)
,_type(kResourceType_Gold)
,_count(INVALID_VALUE)
,_color(Color4B::WHITE)
,_icon(nullptr)
,_iconNode(nullptr)
,_countLabel(nullptr)
,_button(nullptr)
{
    
}

ResourceButton::~ResourceButton()
{
    removeAllChildren();
}

ResourceButton* ResourceButton::create(bool isBigSize, bool animated, bool needResize, ResourceType type, int count, const Color4B& color, const Button::ccWidgetClickCallback& callback)
{
    ResourceButton *p = new (std::nothrow)ResourceButton();
    if(p && p->init(isBigSize, animated, needResize, type, count, color, callback))
    {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool ResourceButton::init(bool isBigSize, bool animated, bool needResize, ResourceType type, int count, const Color4B& color, const Button::ccWidgetClickCallback& callback)
{
    if(Node::init())
    {
        _isBigSize = isBigSize;
        _animated = animated;
        _needResize = needResize;
        _color = color;
        
#if false
        const string csbFile = isBigSize ? "LevelupButton_UI.csb" : "formationButton_UI.csb";
        Node *bn = CSLoader::createNode(csbFile);
        addChild(bn);
        
        cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(csbFile);
        bn->runAction(action);
        action->gotoFrameAndPlay(0, true);
        
        _button = dynamic_cast<Button*>(bn->getChildByTag(121));
        _button->setSwallowTouches(false);
        _button->setPressedActionEnabled(true);
        _button->addClickEventListener(callback);
        
        const Vector<Node*>& children = _button->getChildren();
        for (int i = 0; i < children.size(); ++i)
        {
            Node* child = children.at(i);
            if (child) {
                const Point& position = child->getPosition();
                Node *parent = child->getParent();
                switch (child->getTag()) {
                    case 123:
                    {
                        _countLabel = CocosUtils::create12x30Number(StringUtils::format("%d", count));
                        _countLabel->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
                        _countLabel->setPosition(position);
                        parent->addChild(_countLabel);
                    }
                        break;
                    case 122:
                    {
                        _icon = Sprite::create(StringUtils::format("GameImages/resources/%dS.png", type));
                        _icon->setPosition(position);
                        parent->addChild(_icon);
                    }
                        break;
                        
                    default:
                        break;
                }
            }
        }
        
        setType(type);
        setContentSize(_button->getContentSize());
#else
        addIconNode(type);
        const string& message = StringUtils::format("%d", count);
        const int fontSize = _isBigSize ? DEFAULT_FONT_SIZE : SMALL_FONT_SIZE;
        _countLabel = CocosUtils::createLabel(message, fontSize);
        _countLabel->setTextColor(color);
        addChild(_countLabel);
        
        if (needResize) {
            resize();
        } else {
            Node* icon = _animated ? _iconNode : _icon;
            const Size size = isBigSize ? Size(150, 40) : Size(35, 20);
            setContentSize(size);
            icon->setPosition(Point(size.width / 4, size.height / 2));
            _countLabel->setPosition(Point(size.width * 3 / 4, size.height / 2));
        }
#endif
        
        return true;
    }
    
    return false;
}

ResourceType ResourceButton::getType() const
{
    return _type;
}

int ResourceButton::getCount() const
{
    return _count;
}

void ResourceButton::setType(ResourceType type)
{
    _type = type;
    
    addIconNode(type);
    
    resize();
}

void ResourceButton::setCount(int count)
{
    _count = count;
    
    if (_countLabel)
    {
        _countLabel->setString(StringUtils::format("%d", count));
    }
    resize();
}

void ResourceButton::setEnabled(bool enable)
{
    if (_enabled != enable) {
        _enabled = enable;
        
        const Color4B color = enable ? _color : Color4B::RED;
        _countLabel->setTextColor(color);
    }
}

void ResourceButton::setTouchEventListener(const Button::ccWidgetTouchCallback& callback)
{
    if (_button)
    {
        _button->addTouchEventListener([=](Ref *pSender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::ENDED) {
                SoundManager::getInstance()->playButtonSound();
            }
            callback(pSender, type);
        });
    }
}

void ResourceButton::setClickEventListener(const Button::ccWidgetClickCallback& callback)
{
    if (_button)
    {
        _button->addClickEventListener([=](Ref *pSender) {
            SoundManager::getInstance()->playButtonSound();
            callback(pSender);
        });
    }
}

void ResourceButton::addIconNode(ResourceType type)
{
    if (_animated) {
        Point pos(-1, -1);
        if (_iconNode) {
            pos = _icon->getPosition();
            _iconNode->stopAllActions();
            _iconNode->removeFromParent();
            _iconNode = nullptr;
        }
        
        const string& file(StringUtils::format("%d.csb", type));
        _iconNode = CSLoader::createNode(file);
        addChild(_iconNode);
        timeline::ActionTimeline *action = CSLoader::createTimeline(file);
        _iconNode->runAction(action);
        action->gotoFrameAndPlay(0, true);
        
        if (pos.x > 0) {
            _iconNode->setPosition(pos);
        }
    } else {
        string file;
        if (_isBigSize) {
            file = StringUtils::format("GameImages/resources/icon_%dB.png", type);
        } else {
            file = StringUtils::format("GameImages/resources/icon_%dS.png", type);
        }
        
        if (_icon) {
            _icon->setTexture(file);
        } else {
            _icon = Sprite::create(file);
            addChild(_icon);
        }
    }
}

void ResourceButton::resize()
{
    if (_needResize) {
        if (_button) {
            const float buttonWidth = _button->getContentSize().width;
            const float iconWidth = _icon->getContentSize().width;
            const float labelWidth = _countLabel->getContentSize().width;
            const float x = buttonWidth / 2 - labelWidth / 2;
            _icon->setPositionX(x);
            _countLabel->setPositionX(x + 5.0f + iconWidth / 2 + labelWidth * _countLabel->getAnchorPoint().x);
        } else {
            Node* icon = _animated ? _iconNode : _icon;
            const Size& iconSize = _isBigSize ? Size(36, 36) : Size(24, 20) /*icon->getContentSize()*/;
            const Size& labelSize = _countLabel->getContentSize();
            
            static const float offsetX = _isBigSize ? 5.0f : 2.0f;
            const Size size(iconSize.width + labelSize.width + offsetX, MAX(iconSize.height, labelSize.height));
            
            setContentSize(size);
            
            icon->setPosition(Point(iconSize.width / 2, size.height / 2));
            _countLabel->setPosition(Point(size.width - labelSize.width / 2, size.height / 2));
        }
    }
}