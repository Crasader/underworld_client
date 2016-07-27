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

static const float dialogWidth(291);
static const float dialogArrowWidth(16);
static const float dialogEdge(3);

ChatNode* ChatNode::create(const ChatData* data)
{
    auto ret = new (nothrow) ChatNode();
    if (ret && ret->init(data)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

ChatNode::ChatNode()
:_observer(nullptr)
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

bool ChatNode::init(const ChatData* data)
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        createDialog(true);
        createIcon();
        
        _user = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        _dialogBg->addChild(_user);
        
        _content = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        _content->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        _content->setMaxLineWidth(dialogWidth - (dialogEdge * 2 + dialogArrowWidth));
        _dialogBg->addChild(_content);
        
        _time = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        _dialogBg->addChild(_time);
        
        return true;
    }
    
    return false;
}

void ChatNode::update(const ChatData* data)
{
    if (false) {
        createDialog(true);
    }
    
    if (false) {
        createIcon();
    }
    
    adjust(true);
}

void ChatNode::createDialog(bool isMe)
{
    string file;
    static Size size(dialogWidth, 58);
    static const float offset(6);
    Rect capInsets(offset, offset, size.width - (offset * 2 + dialogArrowWidth), size.height - offset * 2);
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
    static const Size size(68, 68);
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
        _icon->setContentSize(size);
        addChild(_icon);
    }
    
    if (!find) {
        s = Sprite::create(file);
        s->setPosition(Point(size.width / 2, size.height / 2));
        _icon->addChild(s);
    }
    
    if (s) {
        const auto& spriteSize(s->getContentSize());
        s->setScale(size.width / spriteSize.width, size.height / spriteSize.height);
    }
}

void ChatNode::adjust(bool isMe)
{
    // dialog
    static const float space(3);
    const auto& usize(_user->getContentSize());
    const auto& csize(_content->getContentSize());
    const auto& tsize(_time->getContentSize());
    const Size dbgsize(dialogWidth, MAX(usize.height, tsize.height) + csize.height + space + dialogEdge * 2);
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
    
    const float baseOffsetX = isMe ? 0 : dialogArrowWidth;
    topLeftNode->setHorizontalAlignment(TextHAlignment::LEFT);
    topLeftNode->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    topLeftNode->setPosition(baseOffsetX + dialogEdge, dbgsize.height - dialogEdge);
    
    topRightNode->setHorizontalAlignment(TextHAlignment::RIGHT);
    topRightNode->setAnchorPoint(Point::ANCHOR_TOP_RIGHT);
    topRightNode->setPosition(baseOffsetX + (dbgsize.width - dialogEdge), dbgsize.height - dialogEdge);
    
    _content->setPosition(Point(baseOffsetX + dialogEdge + csize.width / 2, dialogEdge + csize.height / 2));
    
    static const float edge(3);
    const auto& isize(_icon->getContentSize());
    const Size size(isize.width + edge + dialogWidth, MAX(isize.height, dbgsize.height));
    setContentSize(size);
    
    if (isMe) {
        _dialogBg->setPosition(Point(dialogWidth / 2, size.height / 2));
        _icon->setPosition(Point(dialogWidth + edge + isize.width / 2, size.height - isize.height / 2));
    } else {
        _icon->setPosition(Point(isize.width / 2, size.height - isize.height / 2));
        _dialogBg->setPosition(Point(isize.width + edge + dialogWidth / 2, size.height / 2));
    }
}
