//
//  PvpLogShareLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "PvpLogShareLayer.h"
#include "ui/CocosGUI.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "PvpLogUI.h"
#include "UniversalButton.h"

using namespace std;
using namespace ui;

static const Size subNodeSize(359, 174);
PvpLogShareLayer* PvpLogShareLayer::create()
{
    auto ret = new (nothrow) PvpLogShareLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

PvpLogShareLayer::PvpLogShareLayer()
:_observer(nullptr)
,_background(nullptr) {}

PvpLogShareLayer::~PvpLogShareLayer()
{
    removeAllChildren();
}

#pragma mark - public
void PvpLogShareLayer::registerObserver(PvpLogShareLayerObserver *observer)
{
    _observer = observer;
}

#pragma mark - LayerColor
bool PvpLogShareLayer::init()
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        
        auto bg = Sprite::create(PvpLogUI::getResourcePath("ui_background_4.png"));
        bg->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(bg);
        _background = bg;
        
        auto subNode = CocosUtils::createBackground("GameImages/public/ui_background_1.png", subNodeSize);
        bg->addChild(subNode);
        
        const auto& size(bg->getContentSize());
        const auto& subBgSize(subNode->getContentSize());
        const float edge((size.width - subBgSize.width) / 2);
        subNode->setPosition(Point(size.width / 2, subBgSize.height / 2 + edge));
        
        CocosUtils::createRedExitButton(bg, [this]() {
            if (_observer) {
                _observer->onPvpLogShareLayerClickedExitButton(this);
            }
        });
        
        auto title = CocosUtils::createLabel("Share", BIG_FONT_SIZE);
        title->setAnchorPoint(Point::ANCHOR_MIDDLE);
        title->setPosition(Point(size.width / 2, (size.height + subBgSize.height + edge) / 2));
        bg->addChild(title);
        
        auto subTitleBg = Sprite::create(PvpLogUI::getResourcePath("ui_tiao_12.png"));
        subNode->addChild(subTitleBg);
        
        auto contentBg = Sprite::create(PvpLogUI::getResourcePath("ui_tiao_11.png"));
        subNode->addChild(contentBg);
        
        const auto& stbgsize(subTitleBg->getContentSize());
        const auto& cbgsize(contentBg->getContentSize());
        const float gapY((subNodeSize.height - (stbgsize.height + cbgsize.height)) / 3);
        subTitleBg->setPosition(Point(subNodeSize.width / 2, subNodeSize.height - (gapY + stbgsize.height / 2)));
        contentBg->setPosition(Point(subNodeSize.width / 2, gapY + cbgsize.height / 2));
        
        auto button = UniversalButton::create(UniversalButton::BSize::Big, UniversalButton::BType::Blue, "Share");
        button->setCallback([this](Ref*) {
            if (_observer) {
                _observer->onPvpLogShareLayerClickedShareButton(this);
            }
        });
        addChild(button);
        button->setPosition(bg->getPosition() - Point(0, size.height / 2));
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(PvpLogShareLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(PvpLogShareLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool PvpLogShareLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void PvpLogShareLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}
