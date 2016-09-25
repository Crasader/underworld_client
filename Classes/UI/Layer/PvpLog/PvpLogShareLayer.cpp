//
//  PvpLogShareLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "PvpLogShareLayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "XButton.h"
#include "SmallBoard.h"
#include "XEditBox.h"

using namespace std;
using namespace ui;

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
,_background(nullptr)
,_editBox(nullptr) {}

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
        auto bg = SmallBoard::create();
        bg->setTitle(LocalHelper::getString("ui_logShare_title"));
        bg->setPosition(Point(winSize.width / 2, winSize.height / 2));
        bg->setExitCallback([this]() {
            if (_observer) {
                _observer->onPvpLogShareLayerClickedExitButton(this);
            }
        });
        addChild(bg);
        _background = bg;
        
        auto subNode = bg->getSubNode();
        const auto& size(bg->getContentSize());
        const auto& subNodeSize(subNode->getContentSize());
        
        auto subTitleBg = Sprite::create(CocosUtils::getResourcePath("ui_tiao_12.png"));
        subNode->addChild(subTitleBg);
        
        const auto& stbgsize(subTitleBg->getContentSize());
        auto subTitle = CocosUtils::createLabel(LocalHelper::getString("ui_logShare_description"), DEFAULT_FONT_SIZE);
        subTitle->setPosition(stbgsize.width / 2, stbgsize.height / 2);
        subTitleBg->addChild(subTitle);
        
        auto contentBg = Sprite::create(CocosUtils::getResourcePath("ui_tiao_11.png"));
        subNode->addChild(contentBg);
        
        static const Size ebBgSize(289, 38);
        const auto& cbgsize(contentBg->getContentSize());
        static const Vec2 editBoxEdge(5, 10);
        const Size esize(cbgsize.width - editBoxEdge.x * 2, cbgsize.height - editBoxEdge.y);
        _editBox = XEditBox::create(esize);
        _editBox->setDelegate(this);
        _editBox->setFontSize(DEFAULT_FONT_SIZE);
        _editBox->setText(LocalHelper::getString("ui_logShare_hint").c_str());
        _editBox->setPosition(Point(cbgsize.width / 2, (cbgsize.height + editBoxEdge.y) / 2));
        contentBg->addChild(_editBox);
        
        const float gapY((subNodeSize.height - (stbgsize.height + cbgsize.height)) / 3);
        subTitleBg->setPosition(Point(subNodeSize.width / 2, subNodeSize.height - (gapY + stbgsize.height / 2)));
        contentBg->setPosition(Point(subNodeSize.width / 2, gapY + cbgsize.height / 2));
        
        auto button = XButton::create(XButton::BSize::Big, XButton::BType::Blue);
        button->setTitleText(LocalHelper::getString("ui_logShare_share"));
        button->addClickEventListener([this](Ref*) {
            if (_observer) {
                _observer->onPvpLogShareLayerClickedShareButton(this, _editBox->getText());
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

void PvpLogShareLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

#pragma mark - EditBoxDelegate
void PvpLogShareLayer::editBoxReturn(EditBox* editBox)
{
    
}
