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
#include "LocalHelper.h"
#include "UniversalButton.h"
#include "MediumBoard.h"

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
        auto board = MediumBoard::create();
        board->setTitle(LocalHelper::getString("ui_rename_title"));
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        
        auto button = UniversalButton::create(UniversalButton::BSize::Big, UniversalButton::BType::Blue, LocalHelper::getString("ui_rename_buttonTitle"));
        button->setCallback([this](Ref*) {
            if (_observer && _editBox) {
                const char* name(_editBox->getText());
                if (name && strlen(name) > 0) {
                    _observer->onRenameLayerRename(this, name);
                }
            }
        });
        board->addChildToMidBottom(button);
        
        auto subNode = board->getSubNode();
        const auto& subBgSize(subNode->getContentSize());
        
        // sub node
        static const float edgeY(10.0f);
        {
            auto s = Sprite::create(CocosUtils::getResourcePath("ui_tiao_8.png"));
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
            auto s = ui::Scale9Sprite::create(CocosUtils::getResourcePath("ui_kuang_6.png"), Rect(0, 0, rs.width, rs.height), Rect(capInsets, capInsets, rs.width - capInsets * 2, rs.height - capInsets * 2));
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

void RenameLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

#pragma mark - EditBoxDelegate
void RenameLayer::editBoxReturn(EditBox* editBox)
{
    
}
