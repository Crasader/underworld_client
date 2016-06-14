//
//  TabButton.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/17.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "TabButton.h"
#include "CocosUtils.h"
#include "SoundManager.h"

using namespace std;
using namespace ui;

static Color4B normalTextColor(Color4B::WHITE);
static Color4B touchedTextColor(Color4B::ORANGE);

TabButton::TabButton()
:_titleLabel(nullptr)
,_button(nullptr)
{
    
}

TabButton::~TabButton()
{
    removeAllChildren();
}

TabButton* TabButton::create(const string& title, const Button::ccWidgetClickCallback& callback)
{
    TabButton *p = new (nothrow)TabButton();
    if(p && p->init(title, callback))
    {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool TabButton::init(const string& title, const Button::ccWidgetClickCallback& callback)
{
    if(Node::init())
    {
        const Size& winSize = Director::getInstance()->getWinSize();
        static const string csbFile(".csb");
        Node *mainNode = CocosUtils::playCSBAnimation(csbFile, false, 0, nullptr);
        mainNode->setPosition(Point(0, winSize.height / 2));
        addChild(mainNode);
        
        Node* root = mainNode;
        const Vector<Node*>& children = root->getChildren();
        for (int i = 0; i < children.size(); ++i)
        {
            Node* child = children.at(i);
            if (child) {
                const int tag = child->getTag();
                if (tag > 0) {
                    switch (tag) {
                        case 20:
                        {
                            Button* button = dynamic_cast<Button*>(child);
                            if (button) {
                                button->addClickEventListener(callback);
                                
                                Node* child = button->getChildByTag(100);
                                if (child) {
                                    Label* label = CocosUtils::createLabel(title, DEFAULT_FONT_SIZE);
                                    label->setTextColor(normalTextColor);
                                    child->addChild(label);
                                    
                                    _titleLabel = label;
                                }
                            }
                            
                            _button = button;
                        }
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        
        return true;
    }
    
    return false;
}

void TabButton::setEnabled(bool enabled)
{
    if (_button) {
        _button->setEnabled(enabled);
        
        if (_titleLabel) {
            if (enabled) {
                _titleLabel->setTextColor(normalTextColor);
            } else {
                _titleLabel->setTextColor(touchedTextColor);
            }
        }
    }
}