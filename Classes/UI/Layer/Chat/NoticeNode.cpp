//
//  NoticeNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/7/25.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "NoticeNode.h"
#include "NoticeData.h"
#include "CocosUtils.h"
#include "ChatUI.h"
#include "UniversalButton.h"

using namespace std;
using namespace ui;

NoticeNode* NoticeNode::create(const NoticeData* data)
{
    auto ret = new (nothrow) NoticeNode();
    if (ret && ret->init(data)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

NoticeNode::NoticeNode()
:_observer(nullptr)
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

bool NoticeNode::init(const NoticeData* data)
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        
        static const Size size(348, 206);
        static const float capInsets(11.0f);
        _bg = Scale9Sprite::create(ChatUI::getResourcePath("ui_kuang_5.png"), Rect(0, 0, size.width, size.height), Rect(capInsets, capInsets, size.width - capInsets * 2, size.height - capInsets * 2));
        addChild(_bg);
        
        _user = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        _user->setAnchorPoint(Point::ANCHOR_MIDDLE);
        _bg->addChild(_user);
        
        _content = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        _content->setAnchorPoint(Point::ANCHOR_MIDDLE);
        _bg->addChild(_content);
        
        _time = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        _time->setAnchorPoint(Point::ANCHOR_MIDDLE);
        _bg->addChild(_time);
        
        _resourceBg = Sprite::create(ChatUI::getResourcePath("ui_tiao_5.png"));
        _bg->addChild(_resourceBg);
        
        const auto& rsize(_resourceBg->getContentSize());
        _content->setMaxLineWidth(rsize.width);
        
        static const float edgeX(3);
        
        // resources
        if (true) {
            float x(edgeX);
            for (int i = 0; i < 3; ++i) {
                static const float space(3);
                auto type = static_cast<ResourceType>(101 + i);
                auto node = createResourceNode(type, 100 + i);
                _resourceBg->addChild(node);
                _resourceNodes.insert(make_pair(type, node));
                
                const auto& nsize(node->getContentSize());
                node->setPosition(x + nsize.width / 2, rsize.height / 2);
                x += nsize.width + space;
            }
        }
        
        // "get" button
        if (true) {
            auto button = UniversalButton::create(UniversalButton::BSize::Small, UniversalButton::BType::Normal, "Get");
            button->setCallback([](Ref*) {
                
            });
            _resourceBg->addChild(button);
            
            const auto& bsize(button->getContentSize());
            button->setPosition(Point(rsize.width - (edgeX + bsize.width / 2), rsize.height / 2));
        }
        
        update(data);
        
        return true;
    }
    
    return false;
}

void NoticeNode::update(const NoticeData* data)
{
    if (data) {
        
    } else {
        
    }
    
    adjust();
}

Node* NoticeNode::createResourceNode(ResourceType type, int count)
{
    auto node = Node::create();
    
    auto icon = Sprite::create(StringUtils::format("GameImages/resources/icon_%dS.png", type));
    node->addChild(icon);
    
    auto label = CocosUtils::createLabel(StringUtils::format("X%d", count), DEFAULT_FONT_SIZE);
    label->setAnchorPoint(Point::ANCHOR_MIDDLE);
    node->addChild(label);
    
    const auto& isize(icon->getContentSize());
    const auto& lsize(label->getContentSize());
    static const float space(3.0f);
    const Size size(isize.width + space + lsize.width, 50);
    node->setAnchorPoint(Point::ANCHOR_MIDDLE);
    node->setContentSize(size);
    
    icon->setPosition(Point(isize.width / 2, size.height / 2));
    label->setPosition(Point(size.width - lsize.width / 2, size.height / 2));
    
    return node;
}

void NoticeNode::adjust()
{
    static const float edge(3);
    static const float space(3);
    const auto& usize(_user->getContentSize());
    const auto& csize(_content->getContentSize());
    const auto& tsize(_time->getContentSize());
    const auto& rsize(_resourceBg->getContentSize());
    const Size size(rsize.width + edge * 2, MAX(usize.height, tsize.height) + csize.height + space * 2);
    _bg->setContentSize(size);
    
    _user->setPosition(Point(edge + usize.width / 2, size.height - (edge + usize.height / 2)));
    _time->setPosition(Point(size.width - (edge + tsize.width / 2), size.height - (edge + tsize.height / 2)));
    _content->setPosition(Point(size.width / 2, edge + rsize.height + space + csize.height / 2));
    _resourceBg->setPosition(Point(size.width / 2, edge + rsize.height / 2));
}
