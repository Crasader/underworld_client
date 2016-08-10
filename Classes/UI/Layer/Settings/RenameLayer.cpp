//
//  RenameLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/7/25.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "RenameLayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "SettingUI.h"
#include "LocalHelper.h"
#include "UniversalButton.h"

using namespace std;
using namespace ui;

RenameLayer* RenameLayer::create()
{
    RenameLayer *ret = new (nothrow) RenameLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

RenameLayer::RenameLayer()
:_observer(nullptr)
,_editBox(nullptr) {}

RenameLayer::~RenameLayer()
{
    removeAllChildren();
}

void RenameLayer::registerObserver(RenameLayerObserver *observer)
{
    _observer = observer;
}

#pragma mark - LayerColor
bool RenameLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        auto bg = Sprite::create(SettingUI::getResourcePath("ui_background_3.png"));
        bg->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(bg);
        
        auto subNode = CocosUtils::createSubBackground(Size(579, 202));
        bg->addChild(subNode);
        
        const auto& size(bg->getContentSize());
        const auto& subBgSize(subNode->getContentSize());
        const float edge((size.height - subBgSize.height) / 2);
        subNode->setPosition(Point(size.width / 2, subBgSize.height / 2 + edge));
        
        // sub node
        static const float edgeY(10.0f);
        {
            auto s = Sprite::create(SettingUI::getResourcePath("ui_tiao_8.png"));
            const auto& size(s->getContentSize());
            s->setPosition(Point(subBgSize.width / 2, subBgSize.height - (size.height / 2 + edgeY)));
            subNode->addChild(s);
            
            auto hint = CocosUtils::createLabel(LocalHelper::getString("ui_rename_hint"), DEFAULT_FONT_SIZE);
            hint->setAnchorPoint(Point::ANCHOR_MIDDLE);
            hint->setPosition(Point(size.width / 2, size.height / 2));
            s->addChild(hint);
        }
        
        {
            auto hint = CocosUtils::createLabel(LocalHelper::getString("ui_rename_inputHint"), DEFAULT_FONT_SIZE);
            hint->setTextColor(Color4B::BLACK);
            hint->setAnchorPoint(Point::ANCHOR_MIDDLE);
            hint->setPosition(Point(subBgSize.width / 2, subBgSize.height / 2));
            subNode->addChild(hint);
        }
        
        {
            static const Size rs(347, 53);
            static const float capInsets(5.0f);
            auto s = ui::Scale9Sprite::create(SettingUI::getResourcePath("ui_kuang_6.png"), Rect(0, 0, rs.width, rs.height), Rect(capInsets, capInsets, rs.width - capInsets * 2, rs.height - capInsets * 2));
            auto eb = ui::EditBox::create(rs, s);
            eb->setDelegate(this);
            eb->setMaxLength(rs.width);
            eb->setReturnType(EditBox::KeyboardReturnType::SEND);
            eb->setInputMode(EditBox::InputMode::ANY);
            eb->setFontSize(DEFAULT_FONT_SIZE);
            eb->setFontColor(Color4B::BLACK);
            const auto& size(eb->getContentSize());
            eb->setPosition(Point(subBgSize.width / 2, size.height / 2 + edgeY));
            subNode->addChild(eb);
            
            _editBox = eb;
        }
        
        CocosUtils::createRedExitButton(bg, [this]() {
            removeFromParent();
        });
        
        auto title = CocosUtils::createLabel(LocalHelper::getString("ui_rename_title"), BIG_FONT_SIZE);
        title->setAnchorPoint(Point::ANCHOR_MIDDLE);
        title->setPosition(Point(size.width / 2, (size.height + subBgSize.height + edge) / 2));
        bg->addChild(title);
        
        auto button = UniversalButton::create(UniversalButton::BSize::Small, UniversalButton::BType::Blue, LocalHelper::getString("ui_rename_buttonTitle"));
        button->setCallback([this](Ref*) {
            if (_observer) {
                _observer->onRenameLayerRename(this, _editBox->getText());
            }
        });
        button->setPosition(Point(size.width / 2, edge / 2));
        bg->addChild(button);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(RenameLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(RenameLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool RenameLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void RenameLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}

#pragma mark - EditBoxDelegate
void RenameLayer::editBoxReturn(EditBox* editBox)
{
    
}
