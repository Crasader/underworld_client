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
#include "XButton.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "RuneGroupData.h"
#include "AbstractProperty.h"
#include "DeckManager.h"

using namespace std;

static const int column(4);
static const float nodeSpaceY(15);
static const float scrollViewSpaceTop(10);
static const float scrollViewSpaceBottom(10);

#pragma mark - RuneBagLayer
RuneBagLayer* RuneBagLayer::create(ObjectUtils::RuneType type, const RuneData* data)
{
    auto ret = new (nothrow) RuneBagLayer();
    if (ret && ret->init(type, data)) {
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
,_scrollView(nullptr)
,_selectedRune(nullptr)
,_nodeSize(Size::ZERO)
,_nodeSpaceX(0)
,_scrollViewMinSize(Size::ZERO) {}

RuneBagLayer::~RuneBagLayer()
{
    removeAllChildren();
}

bool RuneBagLayer::init(ObjectUtils::RuneType type, const RuneData* data)
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        CC_ASSERT(!data || data->getId() <= 0 || type == ObjectUtils::getRuneType(data->getId()));
        const auto& winSize(Director::getInstance()->getWinSize());
        auto board = MediumBoard::create();
        board->setTitle(LocalHelper::getString("ui_rune_title"));
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        
        auto button = XButton::create(XButton::BSize::Big, XButton::BType::Blue);
        button->setTitleText(LocalHelper::getString("ui_rune_pick"));
        button->setCallback([this](Ref*) {
            if (_observer) {
                _observer->onRuneBagLayerSelected(this, _selectedRune);
            }
        });
        board->addChildToMidBottom(button);
        
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
            auto label = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
            label->setDimensions(descSize.width - edge * 2, descSize.height - edge * 2);
            label->setAlignment(TextHAlignment::LEFT, TextVAlignment::TOP);
            label->setTextColor(Color4B::BLACK);
            label->setPosition(descSize.width / 2, descSize.height / 2);
            descBg->addChild(label);
            _description = label;
        }
        
        // name
        {
            auto label = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
            label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
            label->setTextColor(Color4B::BLACK);
            label->setPosition(descEdge + descSize.width / 2, size.height - (size.height - descSize.height) / 4);
            node->addChild(label);
            _name = label;
        }
        
        // effect
        {
            auto et = CocosUtils::createLabel(LocalHelper::getString("ui_rune_currentEffect"), DEFAULT_FONT_SIZE);
            et->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
            et->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            et->setTextColor(Color4B::BLACK);
            et->setPosition(descEdge, (size.height - descSize.height) / 4);
            node->addChild(et);
            
            auto label = CocosUtils::createLabel(data ? data->getProperty()->getName() : "", DEFAULT_FONT_SIZE);
            label->setAlignment(TextHAlignment::RIGHT, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
            label->setTextColor(Color4B::BLACK);
            label->setPosition(descEdge + descSize.width, et->getPositionY());
            node->addChild(label);
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
        
        initRunes(type);
        
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
    if (pSender && _selectedRune != pSender) {
        if (_selectedRune) {
            _selectedRune->select(false);
        }
        
        pSender->select(true);
        update(pSender->getData());
        _selectedRune = pSender;
    }
}

void RuneBagLayer::registerObserver(RuneBagLayerObserver *observer)
{
    _observer = observer;
}

#pragma mark - private
void RuneBagLayer::update(const RuneData* data)
{
    if (_name) {
        _name->setString(data ? data->getProperty()->getName() : "");
    }
    
    if (_description) {
        _description->setString(data ? data->getProperty()->getDescription() : "");
    }
}

void RuneBagLayer::initRunes(ObjectUtils::RuneType type)
{
    if (!_scrollView) {
        return;
    }
    
    const auto& allRunes(DeckManager::getInstance()->getRuneGroups());
    vector<const RuneGroupData*> runes;
    if (ObjectUtils::RuneType::NONE != type) {
        for (auto iter = begin(allRunes); iter != end(allRunes); ++iter) {
            auto data(iter->second);
            if (data->getAmount() > 0 && type == ObjectUtils::getRuneType(data->getId())) {
                runes.push_back(data);
            }
        }
    }
    const auto cnt(runes.size());
    const float height = MAX(_scrollViewMinSize.height, getHeight(cnt, nodeSpaceY) + scrollViewSpaceTop + scrollViewSpaceBottom);
    _scrollView->setInnerContainerSize(Size(_scrollViewMinSize.width, height));
    
    for (int i = 0; i < cnt; ++i) {
        auto node = RuneNode::create(runes.at(i));
        node->registerObserver(this);
        node->setIdx(i);
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
