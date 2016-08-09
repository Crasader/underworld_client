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
#include "SettingUI.h"
#include "LocalHelper.h"
#include "UniversalButton.h"

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
        auto bg = Sprite::create(CocosUtils::getResourcePath("ui_background_4.png"));
        bg->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(bg);
        
        static const Size subNodeSize(359, 174);
        auto subNode = CocosUtils::createBackground(CocosUtils::getResourcePath("ui_background_1.png"), subNodeSize);
        bg->addChild(subNode);
        
        const auto& size(bg->getContentSize());
        const float edge((size.width - subNodeSize.width) / 2);
        subNode->setPosition(Point(size.width / 2, subNodeSize.height / 2 + edge));
        
        CocosUtils::createRedExitButton(bg, [this]() {
            removeFromParent();
        });
        
        auto title = CocosUtils::createLabel(LocalHelper::getString("ui_language_title"), BIG_FONT_SIZE);
        title->setAnchorPoint(Point::ANCHOR_MIDDLE);
        title->setPosition(Point(size.width / 2, (size.height + subNodeSize.height + edge) / 2));
        bg->addChild(title);
        
        auto cancel = UniversalButton::create(UniversalButton::BSize::Small, UniversalButton::BType::Red, LocalHelper::getString("hint_cancel"));
        cancel->setCallback([this](Ref*) {
            removeFromParent();
        });
        cancel->setPosition(Point(size.width / 2, edge / 2));
        subNode->addChild(cancel);
        
        auto confirm = UniversalButton::create(UniversalButton::BSize::Small, UniversalButton::BType::Blue, LocalHelper::getString("hint_confirm"));
        confirm->setCallback([this](Ref*) {
            removeFromParent();
            
            if (_observer) {
                _observer->onLanguageConfirmationLayerConfirm(_idx);
            }
        });
        confirm->setPosition(Point(size.width / 2, edge / 2));
        subNode->addChild(confirm);
        
        static const Vec2 buttonEdge(75, 16);
        const auto& cancelSize(cancel->getContentSize());
        const auto& confirmSize(confirm->getContentSize());
        cancel->setPosition(buttonEdge.x + cancelSize.width / 2, buttonEdge.y + cancelSize.height / 2);
        confirm->setPosition(subNodeSize.width - (buttonEdge.x + confirmSize.width / 2), buttonEdge.y + confirmSize.height / 2);
        
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

void LanguageConfirmationLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}
