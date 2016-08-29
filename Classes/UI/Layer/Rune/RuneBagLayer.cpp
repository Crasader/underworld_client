//
//  RuneBagLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "RuneBagLayer.h"
#include "MediumBoard.h"
#include "PureNode.h"
#include "PureScale9Sprite.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "RuneGroupData.h"
#include "RuneManager.h"

using namespace std;

static int column(4);
static const float nodeSpaceY(15);
static const float scrollViewSpaceTop(10);
static const float scrollViewSpaceBottom(10);

#pragma mark - RuneBagLayer
RuneBagLayer* RuneBagLayer::create()
{
    auto ret = new (nothrow) RuneBagLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

RuneBagLayer::RuneBagLayer()
:_observer(nullptr)
,_name(nullptr)
,_description(nullptr)
,_effect(nullptr)
,_scrollView(nullptr)
,_data(nullptr)
,_nodeSize(Size::ZERO)
,_nodeSpaceX(0)
,_scrollViewMinSize(Size::ZERO) {}

RuneBagLayer::~RuneBagLayer()
{
    removeAllChildren();
}

bool RuneBagLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        auto board = MediumBoard::create();
        board->setTitle("Rune bag");
        board->setButtonTitle(LocalHelper::getString("OK"));
        board->setButtonCallback([this]() {
            
        });
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        
        auto node(board->getSubNode());
        const auto& size(node->getContentSize());
        
        static const float descEdge(3);
        static const Size descSize(217, 134);
        
        // description
        {
            auto descBg = PureScale9Sprite::create(PureScale9Sprite::Type::White);
            descBg->setContentSize(descSize);
            descBg->setPosition(descEdge + descSize.width / 2, size.height / 2);
            node->addChild(descBg);
            
            static const float edge(5);
            auto label = CocosUtils::createLabel("description", DEFAULT_FONT_SIZE);
            label->setDimensions(descSize.width - edge * 2, descSize.height - edge * 2);
            label->setAlignment(TextHAlignment::LEFT, TextVAlignment::TOP);
            label->setTextColor(Color4B::BLACK);
            label->setPosition(descSize.width / 2, descSize.height / 2);
            descBg->addChild(label);
            _description = label;
        }
        
        // name
        {
            auto label = CocosUtils::createLabel("name", DEFAULT_FONT_SIZE);
            label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
            label->setTextColor(Color4B::BLACK);
            label->setPosition(descEdge + descSize.width / 2, size.height - (size.height - descSize.height) / 4);
            node->addChild(label);
            _name = label;
        }
        
        // effect
        {
            auto et = CocosUtils::createLabel("effect:", DEFAULT_FONT_SIZE);
            et->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
            et->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            et->setTextColor(Color4B::BLACK);
            et->setPosition(descEdge, (size.height - descSize.height) / 4);
            node->addChild(et);
            
            auto label = CocosUtils::createLabel("power", DEFAULT_FONT_SIZE);
            label->setAlignment(TextHAlignment::RIGHT, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
            label->setTextColor(Color4B::BLACK);
            label->setPosition(descEdge + descSize.width, et->getPositionY());
            node->addChild(label);
            _effect = label;
        }
        
        // runes
        {
            static const float edgeY(5);
            static const float lineWidth(2);
            auto line = PureNode::createLine(Size(2, size.height - edgeY * 2));
            line->setPosition(descEdge * 2 + descSize.width + lineWidth / 2, size.height / 2);
            node->addChild(line);
            
            _scrollViewMinSize = Size(size.width - (line->getPositionX() + lineWidth / 2), line->getContentSize().height);
            _nodeSize = RuneNode::create(nullptr)->getContentSize();
            _nodeSpaceX = (_scrollViewMinSize.width - _nodeSize.width * column) / (column + 1);
            
            auto sv = ui::ScrollView::create();
            sv->setDirection(ui::ScrollView::Direction::VERTICAL);
            sv->setContentSize(_scrollViewMinSize);
            sv->setPosition(Point(size.width - _scrollViewMinSize.width, edgeY));
            sv->setInnerContainerSize(_scrollViewMinSize);
            sv->setScrollBarEnabled(false);
            sv->addEventListener([this](Ref*, ui::ScrollView::EventType type) {
            });
            node->addChild(sv);
            _scrollView = sv;
        }
        
        initRunes();
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(RuneBagLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(RuneBagLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool RuneBagLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void RuneBagLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

#pragma mark - RuneNodeObserver
void RuneBagLayer::onRuneNodeClicked(RuneNode* pSender)
{
    if (pSender) {
        update(pSender->getData());
    }
}

void RuneBagLayer::registerObserver(RuneBagLayerObserver *observer)
{
    _observer = observer;
}

void RuneBagLayer::update(const RuneData* data)
{
    if (_data != data) {
        _data = data;
        
        if (_name) {
            _name->setString(data ? data->getName() : "");
        }
        
        if (_description) {
            _description->setString(data ? data->getDescription() : "");
        }
        
        if (_effect) {
            _effect->setString(data ? data->getEffect() : "");
        }
    }
}

#pragma mark - private
void RuneBagLayer::initRunes()
{
    if (!_scrollView) {
        return;
    }
    
    auto rm(RuneManager::getInstance());
    const auto cnt(rm->getRuneGroupsCount());
    const float height = MAX(_scrollViewMinSize.height, getHeight(cnt, nodeSpaceY) + scrollViewSpaceTop + scrollViewSpaceBottom);
    _scrollView->setInnerContainerSize(Size(_scrollViewMinSize.width, height));
    
    for (int i = 0; i < cnt; ++i) {
        auto node = RuneNode::create(rm->getRuneGroup(i));
        _scrollView->addChild(node);
        
        auto point = getPosition(i / column, i % column);
        node->setPosition(Point(point.x, height - (point.y + scrollViewSpaceTop)));
    }
}

float RuneBagLayer::getHeight(size_t count, float spaceY) const
{
    if (count > 0) {
        const size_t row = (count - 1) / column + 1;
        return row * (_nodeSize.height + spaceY) - spaceY;
    }
    
    return 0;
}

Point RuneBagLayer::getPosition(int row, int column) const
{
    const float w(_nodeSize.width);
    const float h(_nodeSize.height);
    const float x = (column + 1) * (_nodeSpaceX + w) - w / 2;
    const float y = row * (h + nodeSpaceY) + h / 2;
    return Point(x, y);
}
