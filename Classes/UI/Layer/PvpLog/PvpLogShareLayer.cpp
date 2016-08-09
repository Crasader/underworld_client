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
#include "PvpLogUI.h"
#include "LocalHelper.h"
#include "UniversalButton.h"

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
        
        auto bg = Sprite::create(CocosUtils::getResourcePath("ui_background_4.png"));
        bg->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(bg);
        _background = bg;
        
        static const Size subNodeSize(359, 174);
        auto subNode = CocosUtils::createBackground(CocosUtils::getResourcePath("ui_background_1.png"), subNodeSize);
        bg->addChild(subNode);
        
        const auto& size(bg->getContentSize());
        const float edge((size.width - subNodeSize.width) / 2);
        subNode->setPosition(Point(size.width / 2, subNodeSize.height / 2 + edge));
        
        CocosUtils::createRedExitButton(bg, [this]() {
            if (_observer) {
                _observer->onPvpLogShareLayerClickedExitButton(this);
            }
        });
        
        auto title = CocosUtils::createLabel(LocalHelper::getString("ui_logShare_title"), BIG_FONT_SIZE);
        title->setAnchorPoint(Point::ANCHOR_MIDDLE);
        title->setPosition(Point(size.width / 2, (size.height + subNodeSize.height + edge) / 2));
        bg->addChild(title);
        
        auto subTitleBg = Sprite::create(PvpLogUI::getResourcePath("ui_tiao_12.png"));
        subNode->addChild(subTitleBg);
        
        const auto& stbgsize(subTitleBg->getContentSize());
        auto subTitle = CocosUtils::createLabel(LocalHelper::getString("ui_logShare_description"), DEFAULT_FONT_SIZE);
        subTitle->setPosition(stbgsize.width / 2, stbgsize.height / 2);
        subTitleBg->addChild(subTitle);
        
        auto contentBg = Sprite::create(PvpLogUI::getResourcePath("ui_tiao_11.png"));
        subNode->addChild(contentBg);
        
        static const Size ebBgSize(289, 38);
        static const float capInsetsOffset(6);
        Rect capInsets(capInsetsOffset, capInsetsOffset, ebBgSize.width - capInsetsOffset * 2, ebBgSize.height - capInsetsOffset * 2);
        auto s = Scale9Sprite::create(PvpLogUI::getResourcePath("ui_shuruzikuang.png"), Rect(0, 0, ebBgSize.width, ebBgSize.height), capInsets);
        
        const auto& cbgsize(contentBg->getContentSize());
        static const Vec2 editBoxEdge(5, 10);
        const Size esize(cbgsize.width - editBoxEdge.x * 2, cbgsize.height - editBoxEdge.y);
        _editBox = EditBox::create(esize, s);
        _editBox->setDelegate(this);
        _editBox->setMaxLength(esize.width);
        _editBox->setReturnType(EditBox::KeyboardReturnType::SEND);
        _editBox->setInputMode(EditBox::InputMode::ANY);
        _editBox->setFontSize(DEFAULT_FONT_SIZE);
        _editBox->setFontColor(Color4B::BLACK);
        _editBox->setText(LocalHelper::getString("ui_logShare_hint").c_str());
        contentBg->addChild(_editBox);
        
        const auto& cbsize(contentBg->getContentSize());
        _editBox->setPosition(Point(cbsize.width / 2, cbsize.height / 2 + editBoxEdge.y));
        
        const float gapY((subNodeSize.height - (stbgsize.height + cbgsize.height)) / 3);
        subTitleBg->setPosition(Point(subNodeSize.width / 2, subNodeSize.height - (gapY + stbgsize.height / 2)));
        contentBg->setPosition(Point(subNodeSize.width / 2, gapY + cbgsize.height / 2));
        
        auto button = UniversalButton::create(UniversalButton::BSize::Big, UniversalButton::BType::Blue, LocalHelper::getString("ui_logShare_share"));
        button->setCallback([this](Ref*) {
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

void PvpLogShareLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}

#pragma mark - EditBoxDelegate
void PvpLogShareLayer::editBoxReturn(EditBox* editBox)
{
    
}