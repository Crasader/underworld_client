//
//  ChatNode.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ChatNode.h"
#include "CocosUtils.h"
#include "ChatData.h"
#include "ChatUI.h"

using namespace std;

static const float dialogArrowWidth(16);
static const float dialogEdge(3);
static const float dialogBottomEdge(6);
static const Size iconSize(68, 68);

ChatNode* ChatNode::create(float width, const ChatData* data)
{
    auto ret = new (nothrow) ChatNode();
    if (ret && ret->init(width, data)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

ChatNode::ChatNode()
:_observer(nullptr)
,_width(0)
,_dialogWidth(0)
,_data(nullptr)
,_icon(nullptr)
,_dialogBg(nullptr)
,_user(nullptr)
,_content(nullptr)
,_time(nullptr) {}

ChatNode::~ChatNode()
{
    removeAllChildren();
}

void ChatNode::registerObserver(ChatNodeObserver *observer)
{
    _observer = observer;
}

bool ChatNode::init(float width, const ChatData* data)
{
    if (Node::init()) {
        _width = width;
        
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        static const float nodeSpace(3);
        _dialogWidth = width - (iconSize.width + nodeSpace);
        
        createDialog(true);
        createIcon();
        
        _user = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        _user->setTextColor(Color4B::BLACK);
        _dialogBg->addChild(_user);
        
        _content = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        _content->setTextColor(Color4B::BLACK);
        _content->setHorizontalAlignment(TextHAlignment::LEFT);
        _content->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        _content->setMaxLineWidth(_dialogWidth - (dialogEdge * 2 + dialogArrowWidth));
        _dialogBg->addChild(_content);
        
        _time = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        _time->setTextColor(Color4B::GRAY);
        _dialogBg->addChild(_time);
        
        update(data);
        
        return true;
    }
    
    return false;
}

void ChatNode::update(const ChatData* data)
{
    _data = data;
    
    if (false) {
        createDialog(true);
    }
    
    if (false) {
        createIcon();
    }
    
    if (data) {
        _user->setString(data->getUser());
        _content->setString(data->getMessage());
        _time->setString(data->getFormattedTime());
    }
    
    adjust(true);
}

void ChatNode::createDialog(bool isMe)
{
    string file;
    static const Size size(291, 58);
    static const float offset(5);
    static const float height(8);
    Rect capInsets(offset, size.height - (offset + height), size.width - (offset * 2 + dialogArrowWidth), height);
    if (isMe) {
        file.assign(ChatUI::getResourcePath("ui_liaotiankuang.png"));
    } else {
        file.assign(ChatUI::getResourcePath("ui_liaotiankuang_1.png"));
        capInsets.origin.x += dialogArrowWidth;
    }
    
    if (_dialogBg) {
        _dialogBg->setSpriteFrame(nullptr);
        _dialogBg->setCapInsets(capInsets);
    } else {
        _dialogBg = ui::Scale9Sprite::create(file, Rect(0, 0, size.width, size.height), capInsets);
        _dialogBg->setAnchorPoint(Point::ANCHOR_MIDDLE);
        addChild(_dialogBg);
    }
}

void ChatNode::createIcon()
{
    static const string file("GameImages/public/icon_touxiang_1.png");
    bool find(false);
    Sprite* s(nullptr);
    if (_icon) {
        if (_icon->getChildrenCount() > 0) {
            s = dynamic_cast<Sprite*>(_icon->getChildren().front());
            if (s) {
                s->setTexture(file);
                find = true;
            } else {
                _icon->removeAllChildren();
            }
        }
    } else {
        _icon = Node::create();
        _icon->setAnchorPoint(Point::ANCHOR_MIDDLE);
        _icon->setContentSize(iconSize);
        addChild(_icon);
    }
    
    if (!find) {
        s = Sprite::create(file);
        s->setPosition(Point(iconSize.width / 2, iconSize.height / 2));
        _icon->addChild(s);
    }
    
    if (s) {
        const auto& spriteSize(s->getContentSize());
        s->setScale(iconSize.width / spriteSize.width, iconSize.height / spriteSize.height);
    }
}

void ChatNode::adjust(bool isMe)
{
    // dialog
    const auto& usize(_user->getContentSize());
    const auto& csize(_content->getContentSize());
    const auto& tsize(_time->getContentSize());
    static const float dspace(3);
    const Size dbgsize(_dialogWidth, MAX(usize.height, tsize.height) + csize.height + dspace + dialogEdge + dialogBottomEdge);
    _dialogBg->setContentSize(dbgsize);
    
    Label* topLeftNode(nullptr);
    Label* topRightNode(nullptr);
    
    if (isMe) {
        topLeftNode = _time;
        topRightNode = _user;
    } else {
        topLeftNode = _user;
        topRightNode = _time;
    }
    
    topLeftNode->setHorizontalAlignment(TextHAlignment::LEFT);
    topLeftNode->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    topLeftNode->setPosition((isMe ? 0 : dialogArrowWidth) + dialogEdge, dbgsize.height - dialogEdge);
    
    topRightNode->setHorizontalAlignment(TextHAlignment::RIGHT);
    topRightNode->setAnchorPoint(Point::ANCHOR_TOP_RIGHT);
    topRightNode->setPosition(_dialogWidth - (dialogEdge + (isMe ? dialogArrowWidth : 0)), dbgsize.height - dialogEdge);
    
    _content->setPosition(Point((isMe ? 0 : dialogArrowWidth) + dialogEdge, dialogBottomEdge + csize.height / 2));
    
    const auto& isize(_icon->getContentSize());
    const Size size(_width, MAX(isize.height, dbgsize.height));
    setContentSize(size);
    
    if (isMe) {
        _dialogBg->setPosition(Point(_dialogWidth / 2, size.height / 2));
        _icon->setPosition(Point(_width - isize.width / 2, size.height - isize.height / 2));
    } else {
        _dialogBg->setPosition(Point(_width - _dialogWidth / 2, size.height / 2));
        _icon->setPosition(Point(isize.width / 2, size.height - isize.height / 2));
    }
}
