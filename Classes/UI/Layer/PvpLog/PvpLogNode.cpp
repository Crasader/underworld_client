//
//  PvpLogNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "PvpLogNode.h"
#include "CocosUtils.h"
#include "PvpLogData.h"
#include "UserSimpleData.h"
#include "AbstractData.h"
#include "LocalHelper.h"
#include "PvpResultNode.h"
#include "TrophyGapNode.h"
#include "UserSimpleNode.h"
#include "TowerSimpleNode.h"
#include "CardDeckNode.h"
#include "XButton.h"
#include "SoundManager.h"

using namespace std;
using namespace ui;

struct PvpLogNode::UserInfo {
    UserSimpleNode* user;
    TowerSimpleNode* tower;
    CardDeckNode* deck;
};

static const float edgeDefault(5);
static const float edgeBottom(8);
static float deckNodeWidth(0);
static float userInfoWidth(0);
static const int zorder_top(1);

PvpLogNode* PvpLogNode::create(const PvpLogData* data, bool expand)
{
    auto ret = new (nothrow) PvpLogNode();
    if (ret && ret->init(data, expand)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

PvpLogNode::PvpLogNode()
:_observer(nullptr)
,_isExpanded(false)
,_data(nullptr)
,_background(nullptr)
,_result(nullptr)
,_trophyGap(nullptr)
,_time(nullptr)
,_expandButton(nullptr)
,_top(nullptr)
,_bottom(nullptr)
{
    _userInfos.insert(make_pair(true, new (nothrow) UserInfo()));
    _userInfos.insert(make_pair(false, new (nothrow) UserInfo()));
}

PvpLogNode::~PvpLogNode()
{
    for (auto iter = begin(_userInfos); iter != end(_userInfos); ++iter) {
        CC_SAFE_DELETE(iter->second);
    }
    
    removeAllChildren();
}

void PvpLogNode::registerObserver(PvpLogNodeObserver *observer)
{
    _observer = observer;
}

bool PvpLogNode::init(const PvpLogData* data, bool expand)
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        
        static const string file(CocosUtils::getResourcePath("ui_tiao_10.png"));
        static const Size size(975, 131);
        static const float offset(6);
        static const Rect capInsets(offset, offset, size.width - offset * 2, size.height - offset * 2);
        _background = ui::Scale9Sprite::create(file, Rect(0, 0, size.width, size.height), capInsets);
        _background->setAnchorPoint(Point::ANCHOR_MIDDLE);
        addChild(_background);
        
        static const auto efile(CocosUtils::getResourcePath("icon_jiantou_3.png"));
        _expandButton = Button::create(efile, efile);
        _expandButton->addClickEventListener([this](Ref*) {
            show();
            if (_observer) {
                _observer->onPvpLogNodeExpand(this, _isExpanded);
            }
        });
        addChild(_expandButton, zorder_top);
        
        createTopNode();
        update(data, expand);
        
        return true;
    }
    
    return false;
}

void PvpLogNode::update(const PvpLogData* data, bool expand)
{
    _data = data;
    _isExpanded = expand;
    static bool win(true);
    const string file(CocosUtils::getResourcePath(win ? "ui_tiao_9.png" : "ui_tiao_10.png"));
    _background->setSpriteFrame(Sprite::create(file)->getSpriteFrame());
    _result->setResult(win);
    _userInfos.at(win)->tower->setWin(win);
    _userInfos.at(!win)->tower->setWin(!win);
    adjust();
}

void PvpLogNode::show()
{
    _isExpanded = !_isExpanded;
    adjust();
}

void PvpLogNode::createTopNode()
{
    if (!_top) {
        auto node = Node::create();
        node->setAnchorPoint(Point::ANCHOR_MIDDLE);
        node->setContentSize(Size(Width, FoldedHeight));
        const auto& size(node->getContentSize());
        
        // replay
        auto replay = XButton::create(XButton::BSize::Big, XButton::BType::Green);
        replay->setTitleText(LocalHelper::getString("ui_log_replay"));
        replay->setCallback([this](Ref*) {
            if (_observer) {
                _observer->onPvpLogNodeReplay(_data);
            }
        });
        node->addChild(replay);
        const auto& rsize(replay->getContentSize());
        replay->setPosition(Point(size.width - (edgeDefault + rsize.width / 2), size.height - (edgeDefault + rsize.height / 2)));
        
        // share
        auto share = XButton::create(XButton::BSize::Big, XButton::BType::Blue);
        share->setTitleText(LocalHelper::getString("ui_log_share"));
        share->setCallback([this](Ref*) {
            if (_observer) {
                _observer->onPvpLogNodeShare(_data);
            }
        });
        node->addChild(share);
        const auto& ssize(share->getContentSize());
        share->setPosition(Point(size.width - (edgeDefault + ssize.width / 2), edgeBottom + ssize.height / 2));
        
        static const float spaceRight(8);
        userInfoWidth = size.width - (edgeDefault + MAX(rsize.width, ssize.width) + spaceRight);
        
        // top-left
        {
            _result = PvpResultNode::create();
            node->addChild(_result);
            
            _trophyGap = TrophyGapNode::create();
            _trophyGap->setCount(-29);
            _trophyGap->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            node->addChild(_trophyGap);
            
            static const float space(20);
            const auto& rsize(_result->getContentSize());
            _result->setPosition(edgeDefault + rsize.width / 2, size.height - (edgeDefault + rsize.height / 2));
            _trophyGap->setPosition(edgeDefault + rsize.width + space, _result->getPositionY());
        }
        
        // top-right
        {
            const auto& time = CocosUtils::getFormattedTime(1234);
            _time = CocosUtils::createLabel(time, SMALL_FONT_SIZE, DEFAULT_NUMBER_FONT);
            _time->setAlignment(cocos2d::TextHAlignment::RIGHT, cocos2d::TextVAlignment::CENTER);
            _time->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
            node->addChild(_time);
            
            const auto& tsize(_time->getContentSize());
            _time->setPosition(userInfoWidth, size.height - (edgeDefault + tsize.height / 2));
        }
        
        // middle
        {
            const float midPos((edgeDefault + userInfoWidth) / 2);
            auto icon = Sprite::create(CocosUtils::getResourcePath("icon_pvp_1.png"));
            node->addChild(icon);
            icon->setPosition(midPos, size.height / 2);
        }
        
        {
            // user info
            for (auto iter = begin(_userInfos); iter != end(_userInfos); ++iter) {
                auto user(UserSimpleNode::create());
                user->setIsHome(iter->first);
                user->setIsMe(false);
                node->addChild(user);
                
                auto tower(TowerSimpleNode::create());
                node->addChild(tower);
                
                auto& info(iter->second);
                info->user = user;
                info->tower = tower;
            }
            
            static const float space(20);
            
            // left
            {
                auto userInfo = _userInfos.at(true);
                auto user(userInfo->user);
                auto tower(userInfo->tower);
                
                const auto& usize(user->getContentSize());
                const auto& tsize(tower->getContentSize());
                user->setPosition(edgeDefault + usize.width / 2, edgeBottom + usize.height / 2);
                tower->setPosition(edgeDefault + usize.width + space + tsize.width / 2, user->getPositionY());
                
                deckNodeWidth = usize.width + tsize.width + space;
            }
            
            // right
            {
                auto userInfo = _userInfos.at(false);
                auto user(userInfo->user);
                auto tower(userInfo->tower);
                
                const auto& usize(user->getContentSize());
                const auto& tsize(tower->getContentSize());
                user->setPosition(userInfoWidth - usize.width / 2, edgeBottom + usize.height / 2);
                tower->setPosition(userInfoWidth - (usize.width + space + tsize.width / 2), user->getPositionY());
            }
        }
        
        _background->addChild(node);
        _top = node;
    }
}

void PvpLogNode::createBottomNode()
{
    if (!_bottom) {
        auto node = Node::create();
        node->setAnchorPoint(Point::ANCHOR_MIDDLE);
        node->setContentSize(Size(userInfoWidth, ExpandedHeight - FoldedHeight));
        const auto& size(node->getContentSize());        
        const Size deckSize(deckNodeWidth, size.height - (edgeDefault + edgeBottom));
        static const size_t column(5);
        static const size_t row(2);
        for (auto iter = begin(_userInfos); iter != end(_userInfos); ++iter) {
            auto deck(CardDeckNode::create(deckSize, column, row));
            vector<AbstractData*> vec;
            for (int i = 0; i < 10; ++i) {
                vec.push_back(nullptr);
            }
            deck->update(vec);
            node->addChild(deck);
            iter->second->deck = deck;
        }
        
        // left
        {
            auto deck(_userInfos.at(true)->deck);
            const auto& dsize(deck->getContentSize());
            deck->setPosition(edgeDefault + dsize.width / 2, edgeBottom + dsize.height / 2);
        }
        
        // right
        {
            auto deck(_userInfos.at(false)->deck);
            const auto& dsize(deck->getContentSize());
            deck->setPosition(size.width - dsize.width / 2, edgeBottom + dsize.height / 2);
        }
        
        _background->addChild(node);
        _bottom = node;
    }
}

void PvpLogNode::removeBottomNode()
{
    if (_bottom) {
        _bottom->removeFromParent();
        _bottom = nullptr;
    }
}

void PvpLogNode::adjust()
{
    Size size(Width, 0);
    if (_isExpanded) {
        createBottomNode();
        size.height = ExpandedHeight;
    } else {
        removeBottomNode();
        size.height = FoldedHeight;
    }
    _background->setContentSize(size);
    setContentSize(size);
    _background->setPosition(Point(size.width / 2, size.height / 2));
    
    const auto& esize(_expandButton->getContentSize());
    _expandButton->setPosition(Point((edgeDefault + userInfoWidth) / 2, edgeBottom + esize.height / 2));
    _expandButton->setScaleY(_isExpanded ? -1 : 1);
    
    if (_top) {
        const auto& nsize(_top->getContentSize());
        _top->setPosition(nsize.width / 2, size.height - nsize.height / 2);
    }
    
    if (_bottom) {
        const auto& nsize(_bottom->getContentSize());
        _bottom->setPosition(nsize.width / 2, nsize.height / 2);
    }
}
