//
//  CardOpNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/12.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardOpNode.h"
#include "DeckCard.h"
#include "CocosUtils.h"
#include "UniversalButton.h"

using namespace std;

static const float edgeX(5);
static const float edgeTop(12);
static const float edgeBottom(8);
Size CardOpNode::_hintMinSize = Size(DeckCard::Width + edgeX * 2, DeckCard::Height + edgeTop + edgeBottom);

CardOpNode* CardOpNode::create()
{
    auto ret = new (nothrow) CardOpNode();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CardOpNode::CardOpNode()
:_observer(nullptr)
,_icon(nullptr)
,_hint(nullptr)
,_touchInvalid(false) {}

CardOpNode::~CardOpNode()
{
    removeAllChildren();
}

bool CardOpNode::init()
{
    if (ui::Widget::init())
    {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        
        static const Size size(110, 233);
        static const float capInsets(18.0f);
        static Rect capInset(capInsets, capInsets, size.width - capInsets * 2, size.height - capInsets * 2);
        _hint = ui::Scale9Sprite::create(BattleDeckUI::getResourcePath("ui_kuang_11.png"), Rect(Point::ZERO, size), capInset);
        addChild(_hint);
        
        _icon = DeckCard::create(0);
        _hint->addChild(_icon);
        
        resetPositions();
        setTouchEnabled(true);
        setSwallowTouches(true);
        CocosUtils::fixWidgetTouchEvent(this, _touchInvalid, nullptr, [this](Ref*) {
            if (_observer) {
                _observer->onCardOpNodeClicked();
            }
        });
        
        return true;
    }
    
    return false;
}

void CardOpNode::registerObserver(CardOpNodeObserver *observer)
{
    _observer = observer;
}

void CardOpNode::setCard(const AbstractData* data)
{
    if (_icon) {
        _icon->update(data);
    }
}

void CardOpNode::setTypes(const vector<DeckCardOpType>& types)
{
    if (_opTypes != types) {
        _opTypes = types;
        
        // hide all buttons
        for (auto iter = begin(_buttons); iter != end(_buttons); ++iter) {
            iter->second->setVisible(false);
        }
        
        for (const auto& type : types) {
            if (_buttons.find(type) != end(_buttons)) {
                _buttons.at(type)->setVisible(true);
            } else {
                auto button = BattleDeckUI::createButton(type, [this, type]() {
                    if (_observer) {
                        _observer->onCardOpNodeClickedButton(type, getCardData());
                    }
                });
                _hint->addChild(button);
                _buttons.insert(make_pair(type, button));
            }
        }
        
        resetPositions();
    }
}

const AbstractData* CardOpNode::getCardData() const
{
    if (_icon) {
        return _icon->getCardData();
    }
    
    return nullptr;
}

void CardOpNode::resetPositions()
{
    static const float space(5);
    float h(0);
    for (auto iter = begin(_buttons); iter != end(_buttons); ++iter) {
        auto button(iter->second);
        if (button && button->isVisible()) {
            h += space + button->getContentSize().height;
        }
    }
    
    const Size size(_hintMinSize + Size(0, h));
    setContentSize(size);
    _hint->setContentSize(size);
    _hint->setPosition(size.width / 2, size.height / 2);
    
    static const float x(_hintMinSize.width / 2);
    float y(edgeBottom);
    for (auto iter = _opTypes.rbegin(); iter != _opTypes.rend(); ++iter) {
        auto button = _buttons.at(*iter);
        const float height(button->getContentSize().height);
        button->setPosition(x, y + height / 2);
        y += height + space;
    }
    
    _icon->setPosition(Point(x, y + _icon->getContentSize().height / 2));
}
