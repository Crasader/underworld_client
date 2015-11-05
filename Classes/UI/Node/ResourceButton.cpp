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
:_type(kResourceType_Gold)
,_count(INVALID_VALUE)
,_icon(nullptr)
,_countLabel(nullptr)
,_button(nullptr)
{
    
}

ResourceButton::~ResourceButton()
{
    removeFromParent();
}

ResourceButton* ResourceButton::create(bool isBigSize, ResourceType type, int count, const Button::ccWidgetClickCallback& callback)
{
    ResourceButton *p = new (std::nothrow)ResourceButton();
    if(p && p->init(isBigSize, type, count, callback))
    {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool ResourceButton::init(bool isBigSize, ResourceType type, int count, const Button::ccWidgetClickCallback& callback)
{
    if(Node::init())
    {
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
        Label* label = CocosUtils::createLabel(StringUtils::format("%d", count), DEFAULT_FONT_SIZE);
        addChild(label);
        setContentSize(label->getContentSize());
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
    
    if (_icon)
    {
        string fileName = StringUtils::format("GameImages/resources/%dS.png", type);
        assert(FileUtils::getInstance()->isFileExist(fileName));
        _icon->setTexture(fileName);
    }
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

void ResourceButton::resize()
{
    const float buttonWidth = _button->getContentSize().width;
    const float iconWidth = _icon->getContentSize().width;
    const float labelWidth = _countLabel->getContentSize().width;
    const float x = buttonWidth / 2 - labelWidth / 2;
    _icon->setPositionX(x);
    _countLabel->setPositionX(x + 5.0f + iconWidth / 2 + labelWidth * _countLabel->getAnchorPoint().x);
}