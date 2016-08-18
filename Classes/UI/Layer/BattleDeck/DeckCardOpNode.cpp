//
//  DeckCardOpNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/12.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "DeckCardOpNode.h"
#include "DeckCard.h"
#include "UniversalButton.h"
#include "CardSimpleData.h"
#include "BattleDeckUI.h"
#include "LocalHelper.h"

using namespace std;

static const float edgeX(5);
static const float edgeTop(12);
static const float edgeBottom(8);
Size DeckCardOpNode::_hintMinSize = Size(DeckCard::Width + edgeX * 2, DeckCard::Height + edgeTop + edgeBottom);

DeckCardOpNode* DeckCardOpNode::create()
{
    auto ret = new (nothrow) DeckCardOpNode();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

DeckCardOpNode::DeckCardOpNode()
:_observer(nullptr)
,_icon(nullptr)
,_hint(nullptr)
,_touchInvalid(false) {}

DeckCardOpNode::~DeckCardOpNode()
{
    removeAllChildren();
}

bool DeckCardOpNode::init()
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
        addTouchEventListener([this](Ref *pSender, ui::Widget::TouchEventType type) {
            auto widget = dynamic_cast<ui::Widget*>(pSender);
            if (type == ui::Widget::TouchEventType::BEGAN) {
                _touchInvalid = false;
            } else if (type == ui::Widget::TouchEventType::MOVED) {
                if (!_touchInvalid) {
                    const auto& mp(widget->getTouchMovePosition());
                    const auto& bp(widget->getTouchBeganPosition());
                    static const float offset(40);
                    if (abs(mp.x - bp.x) >= offset || abs(mp.y - bp.y) >= offset) {
                        _touchInvalid = true;
                    }
                }
            } else if (type == ui::Widget::TouchEventType::ENDED) {
                if (!_touchInvalid && _observer) {
                    _observer->onDeckCardOpNodeClicked();
                }
            }
        });
        
        return true;
    }
    
    return false;
}

void DeckCardOpNode::registerObserver(DeckCardOpNodeObserver *observer)
{
    _observer = observer;
}

void DeckCardOpNode::setCard(int cardId)
{
    _icon->update(cardId);
}

void DeckCardOpNode::setTypes(const vector<DeckCardOpType>& types)
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
                auto button = createButton(type);
                _hint->addChild(button);
                _buttons.insert(make_pair(type, button));
            }
        }
        
        resetPositions();
    }
}

int DeckCardOpNode::getCardId() const
{
    if (_icon) {
        return _icon->getCardId();
    }
    
    return 0;
}

UniversalButton* DeckCardOpNode::createButton(DeckCardOpType opType) const
{
    static const UniversalButton::BSize size(UniversalButton::BSize::Small);
    
    UniversalButton::BType type(UniversalButton::BType::Blue);
    string title;
    switch (opType) {
        case DeckCardOpType::Upgrade: {
            type = UniversalButton::BType::Green;
            title = LocalHelper::getString("ui_deck_upgrade");
        }
            break;
        case DeckCardOpType::Use: {
            type = UniversalButton::BType::Green;
            title = LocalHelper::getString("ui_deck_use");
        }
            break;
        case DeckCardOpType::Info: {
            title = LocalHelper::getString("ui_deck_info");
        }
            break;
        case DeckCardOpType::Move: {
            type = UniversalButton::BType::Green;
            title = LocalHelper::getString("ui_deck_move");
        }
            break;
        default:
            break;
    }
    
    auto button = UniversalButton::create(size, type, title);
    button->setCallback([opType, this](Ref*) {
        if (_observer) {
            _observer->onDeckCardOpNodeClickedButton(opType, getCardId());
        }
    });
    return button;
}

void DeckCardOpNode::resetPositions()
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
