//
//  RuneCircle.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/29.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "RuneCircle.h"
#include "RuneData.h"
#include "DevelopUI.h"
#include "CocosUtils.h"

using namespace std;

RuneCircle* RuneCircle::create()
{
    auto ret = new (nothrow) RuneCircle();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

RuneCircle::RuneCircle()
:_observer(nullptr)
,_circle(nullptr) {}

RuneCircle::~RuneCircle()
{
    removeAllChildren();
}

bool RuneCircle::init()
{
    if (Node::init())
    {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        
        auto bg = Sprite::create(DevelopUI::getResourcePath("ui_line_3.png"));
        addChild(bg);
        _circle = bg;
        
        const auto& size(bg->getContentSize());
        setContentSize(size);
        bg->setPosition(size.width / 2, size.height / 2);
        
        return true;
    }
    
    return false;
}

void RuneCircle::registerObserver(RuneCircleObserver *observer)
{
    _observer = observer;
}

void RuneCircle::setData(const vector<const RuneData*>& datas)
{
    for (int i = 0; i < datas.size(); ++i) {
        auto node = RuneNode::create(datas.at(i));
        node->registerObserver(this);
        _circle->addChild(node);
        _nodes.push_back(node);
    }
    
    const auto cnt(_nodes.size());
    if (cnt < 4) {
        const auto& size(_circle->getContentSize());
        
        // 0
        static float offset(20);
        {
            auto node(_nodes.at(0));
            node->setPosition(Point(size.width / 2, size.height - (node->getContentSize().height / 2 - offset)));
        }
        
        //
        if (2 == cnt) {
            auto node(_nodes.at(1));
            node->setPosition(Point(size.width / 2, node->getContentSize().height / 2 - offset));
        } else if (3 == cnt) {
            auto node(_nodes.at(1));
            node->setPosition(Point(size.width / 4 - offset, size.height / 4));
            
            node = _nodes.at(2);
            node->setPosition(Point(size.width * 3 / 4 + offset, size.height / 4));
        }
    }
}

#pragma mark - RuneNodeObserver
void RuneCircle::onRuneNodeClicked(RuneNode* pSender)
{
    if (pSender && _observer) {
        _observer->onRuneCircleClicked(pSender->getData());
    }
}
