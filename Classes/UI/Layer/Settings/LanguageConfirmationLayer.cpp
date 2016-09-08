//
//  LanguageConfirmationLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/9.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "LanguageConfirmationLayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "XButton.h"
#include "SmallBoard.h"

using namespace std;

LanguageConfirmationLayer* LanguageConfirmationLayer::create(ssize_t idx)
{
    auto ret = new (nothrow) LanguageConfirmationLayer();
    if (ret && ret->init(idx)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

LanguageConfirmationLayer::LanguageConfirmationLayer()
:_observer(nullptr)
,_idx(CC_INVALID_INDEX) {}

LanguageConfirmationLayer::~LanguageConfirmationLayer()
{
    removeAllChildren();
}

void LanguageConfirmationLayer::registerObserver(LanguageConfirmationLayerObserver *observer)
{
    _observer = observer;
}

#pragma mark - LayerColor
bool LanguageConfirmationLayer::init(ssize_t idx)
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        _idx = idx;
        
        const auto& winSize(Director::getInstance()->getWinSize());
        auto bg = SmallBoard::create();
        bg->setTitle(LocalHelper::getString("ui_language_title"));
        bg->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(bg);
        
        auto subNode = bg->getSubNode();
        const auto& size(bg->getContentSize());
        const auto& subNodeSize(subNode->getContentSize());
        const float edge((size.width - subNodeSize.width) / 2);
        
        auto cancel = XButton::create(XButton::BSize::Small, XButton::BType::Red);
        cancel->setTitleText(LocalHelper::getString("hint_cancel"));
        cancel->setCallback([this](Ref*) {
            removeFromParent();
        });
        cancel->setPosition(Point(size.width / 2, edge / 2));
        subNode->addChild(cancel);
        
        auto confirm = XButton::create(XButton::BSize::Small, XButton::BType::Blue);
        confirm->setTitleText(LocalHelper::getString("hint_confirm"));
        confirm->setCallback([this](Ref*) {
            if (_observer) {
                _observer->onLanguageConfirmationLayerConfirm(this, _idx);
            }
        });
        confirm->setPosition(Point(size.width / 2, edge / 2));
        subNode->addChild(confirm);
        
        static const Vec2 buttonEdge(75, 16);
        const auto& cancelSize(cancel->getContentSize());
        const auto& confirmSize(confirm->getContentSize());
        cancel->setPosition(Point(buttonEdge.x + cancelSize.width / 2, buttonEdge.y + cancelSize.height / 2));
        confirm->setPosition(Point(subNodeSize.width - (buttonEdge.x + confirmSize.width / 2), buttonEdge.y + confirmSize.height / 2));
        
        auto hint = CocosUtils::createLabel(LocalHelper::getString("ui_language_hint"), DEFAULT_FONT_SIZE);
        hint->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
        hint->setAnchorPoint(Point::ANCHOR_MIDDLE);
        hint->setTextColor(Color4B::BLACK);
        subNode->addChild(hint);
        
        const float height(subNodeSize.height - (buttonEdge.y + MAX(cancelSize.height, confirmSize.height)));
        hint->setPosition(subNodeSize.width / 2, subNodeSize.height - height / 2);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(LanguageConfirmationLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(LanguageConfirmationLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool LanguageConfirmationLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void LanguageConfirmationLayer::onTouchEnded(Touch *touch, Event *unused_event) {}
