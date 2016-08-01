//
//  NoticeNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/7/25.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "NoticeNode.h"
#include "CocosUtils.h"
#include "ChatData.h"
#include "ObjectBriefData.h"
#include "ChatUI.h"
#include "ObjectBriefNode.h"
#include "UniversalButton.h"

using namespace std;
using namespace ui;

static const unsigned int resourceMaxCount(4);
static const float resourceNodeEdgeX(3);

NoticeNode* NoticeNode::create(float width, const ChatData* data)
{
    auto ret = new (nothrow) NoticeNode();
    if (ret && ret->init(width, data)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

NoticeNode::NoticeNode()
:_observer(nullptr)
,_width(0)
,_bg(nullptr)
,_user(nullptr)
,_content(nullptr)
,_time(nullptr)
,_resourceBg(nullptr) {}

NoticeNode::~NoticeNode()
{
    removeAllChildren();
}

void NoticeNode::registerObserver(NoticeNodeObserver *observer)
{
    _observer = observer;
}

bool NoticeNode::init(float width, const ChatData* data)
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        _width = width;
        
        static const Size size(348, 206);
        static const float capInsets(11.0f);
        _bg = Scale9Sprite::create(ChatUI::getResourcePath("ui_kuang_5.png"), Rect(0, 0, size.width, size.height), Rect(capInsets, capInsets, size.width - capInsets * 2, size.height - capInsets * 2));
        addChild(_bg);
        
        _user = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        _user->setTextColor(Color4B::BLACK);
        _user->setHorizontalAlignment(TextHAlignment::LEFT);
        _user->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
        _bg->addChild(_user);
        
        _content = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        _content->setTextColor(Color4B::BLACK);
        _content->setHorizontalAlignment(TextHAlignment::LEFT);
        _content->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        _bg->addChild(_content);
        
        _time = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        _time->setTextColor(Color4B::BLUE);
        _time->setHorizontalAlignment(TextHAlignment::RIGHT);
        _time->setAnchorPoint(Point::ANCHOR_TOP_RIGHT);
        _bg->addChild(_time);
        
        _resourceBg = Sprite::create(ChatUI::getResourcePath("ui_tiao_5.png"));
        _bg->addChild(_resourceBg);
        
        const auto& rsize(_resourceBg->getContentSize());
        _content->setMaxLineWidth(rsize.width);
        
        // resources
        if (true) {
            for (int i = 0; i < resourceMaxCount; ++i) {
                auto node = ObjectBriefNode::create(nullptr);
                _resourceBg->addChild(node);
                _resourceNodes.push_back(node);
            }
        }
        
        // "get" button
        if (true) {
            auto button = UniversalButton::create(UniversalButton::BSize::Small, UniversalButton::BType::Normal, "Get");
            button->setCallback([](Ref*) {
                
            });
            _resourceBg->addChild(button);
            
            const auto& bsize(button->getContentSize());
            button->setPosition(Point(rsize.width - (resourceNodeEdgeX + bsize.width / 2), rsize.height / 2));
        }
        
        update(data);
        
        return true;
    }
    
    return false;
}

void NoticeNode::update(const ChatData* data)
{
    if (data) {
        _user->setString(data->getUser());
        _content->setString(data->getMessage());
        _time->setString(data->getFormattedTime());
        const auto& rewards(data->getRewards());
        auto cnt(rewards.size());
        _resourceBg->setVisible(cnt > 0);
        
        float x(resourceNodeEdgeX);
        const auto& rsize(_resourceBg->getContentSize());
        for (int i = 0; i < cnt; ++i) {
            if (i < resourceMaxCount) {
                auto node(_resourceNodes.at(i));
                node->setVisible(true);
                node->update(rewards.at(i));
                
                static const float space(3);
                const auto& nsize(node->getContentSize());
                node->setPosition(x + nsize.width / 2, rsize.height / 2);
                x += nsize.width + space;
            } else {
                break;
            }
        }
        
        for (auto i = cnt; i < resourceMaxCount; ++i) {
            _resourceNodes.at(i)->setVisible(false);
        }
    } else {
        static const string empty("");
        _user->setString(empty);
        _content->setString(empty);
        _time->setString(empty);
        _resourceBg->setVisible(false);
    }
    
    adjust();
}

void NoticeNode::adjust()
{
    static const float edge(3);
    static const float space(3);
    const auto& usize(_user->getContentSize());
    const auto& csize(_content->getContentSize());
    const auto& tsize(_time->getContentSize());
    const auto& rsize(_resourceBg->getContentSize());
    const bool rvisible(_resourceBg->isVisible());
    const float rh(rvisible ? (rsize.height + space) : 0);
    const Size size(_width, MAX(usize.height, tsize.height) + csize.height + space + rh + edge * 2);
    _bg->setContentSize(size);
    
    _user->setPosition(Point(edge, size.height - edge));
    _time->setPosition(Point(size.width - edge, size.height - edge));
    _content->setPosition(Point(edge, edge + rh + csize.height / 2));
    
    if (rvisible) {
        _resourceBg->setPosition(Point(size.width / 2, edge + rsize.height / 2));
        _resourceBg->setScaleX((_width - edge * 2) / rsize.width);
    }
    
    setContentSize(size);
    _bg->setPosition(size.width / 2, size.height / 2);
}
