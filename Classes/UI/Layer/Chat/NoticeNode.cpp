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
#include "LocalHelper.h"
#include "ObjectBriefNode.h"
#include "XButton.h"

using namespace std;
using namespace ui;

static const unsigned int resourceMaxCount(4);
static const float resourceNodeEdgeX(3);

NoticeNode* NoticeNode::create(ChatType type, float width, const ChatData* data)
{
    auto ret = new (nothrow) NoticeNode();
    if (ret && ret->init(type, width, data)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

NoticeNode::NoticeNode()
:_observer(nullptr)
,_type(ChatType::Mail)
,_width(0)
,_data(nullptr)
,_bg(nullptr)
,_user(nullptr)
,_content(nullptr)
,_time(nullptr)
,_resourceBg(nullptr)
,_button(nullptr)
,_touchInvalid(false) {}

NoticeNode::~NoticeNode()
{
    removeAllChildren();
}

void NoticeNode::registerObserver(NoticeNodeObserver *observer)
{
    _observer = observer;
}

bool NoticeNode::init(ChatType type, float width, const ChatData* data)
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        _type = type;
        _width = width;
        _data = data;
        
        static const Size size(348, 206);
        static const float capInsets(11.0f);
        _bg = Scale9Sprite::create(CocosUtils::getResourcePath("ui_kuang_5.png"), Rect(0, 0, size.width, size.height), Rect(capInsets, capInsets, size.width - capInsets * 2, size.height - capInsets * 2));
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
        _content->setMaxLineWidth(size.width - 10);
        _bg->addChild(_content);
        
        _time = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        _time->setTextColor(Color4B::BLUE);
        _time->setHorizontalAlignment(TextHAlignment::RIGHT);
        _time->setAnchorPoint(Point::ANCHOR_TOP_RIGHT);
        _bg->addChild(_time);
        
        if (ChatType::Mail == type) {
            _resourceBg = Sprite::create(CocosUtils::getResourcePath("ui_tiao_5.png"));
            _bg->addChild(_resourceBg);
            
            for (int i = 0; i < resourceMaxCount; ++i) {
                auto node = ObjectBriefNode::create();
                _resourceBg->addChild(node);
                _resourceNodes.push_back(node);
            }
            
            _button = XButton::create(XButton::BSize::Small, XButton::BType::Blue);
            _resourceBg->addChild(_button);
            
            const auto& bsize(_button->getContentSize());
            const auto& rsize(_resourceBg->getContentSize());
            _button->setPosition(Point(rsize.width - (resourceNodeEdgeX + bsize.width / 2), rsize.height / 2));
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
        
        if (_resourceBg) {
            const auto& rewards(data->getRewards());
            auto cnt(rewards.size());
            if (cnt > 0) {
                _button->setType(XButton::BType::Blue);
                _button->setTitleText(LocalHelper::getString("ui_chat_mail_get"));
            } else {
                _button->setType(XButton::BType::Red);
                _button->setTitleText(LocalHelper::getString("ui_chat_mail_delete"));
            }
            
            CocosUtils::fixWidgetTouchEvent(_button, _touchInvalid, [cnt, this](Ref*) {
                if (_observer) {
                    if (cnt > 0) {
                        _observer->onNoticeNodeGet(_data);
                    } else {
                        _observer->onNoticeNodeDelete(_data);
                    }
                }
            });
            
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
        }
    } else {
        static const string empty("");
        _user->setString(empty);
        _content->setString(empty);
        _time->setString(empty);
        if (_resourceBg) {
            _resourceBg->removeFromParent();
            _resourceBg = nullptr;
        }
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
    const float rh(_resourceBg ? (_resourceBg->getContentSize().height + space) : 0);
    const Size size(_width, MAX(usize.height, tsize.height) + csize.height + space + rh + edge * 2);
    _bg->setContentSize(size);
    
    _user->setPosition(Point(edge, size.height - edge));
    _time->setPosition(Point(size.width - edge, size.height - edge));
    _content->setPosition(Point(edge, edge + rh + csize.height / 2));
    
    if (_resourceBg) {
        const auto& rsize(_resourceBg->getContentSize());
        _resourceBg->setPosition(Point(size.width / 2, edge + rsize.height / 2));
        _resourceBg->setScaleX((_width - edge * 2) / rsize.width);
    }
    
    setContentSize(size);
    _bg->setPosition(size.width / 2, size.height / 2);
}
