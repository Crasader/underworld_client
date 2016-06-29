//
//  TalentInfoNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/6/29.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "TalentInfoNode.h"
#include "ui/CocosGUI.h"
#include "CocosUtils.h"
#include "TalentBriefNode.h"

using namespace ui;
using namespace std;

static const unsigned int TalentsMaxCount(4);

TalentInfoNode* TalentInfoNode::create(const Callback& callback)
{
    TalentInfoNode *ret = new (nothrow) TalentInfoNode();
    if (ret && ret->init(callback))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

TalentInfoNode::TalentInfoNode()
:_cost(0)
,_callback(nullptr)
,_costLabel(nullptr) {}

TalentInfoNode::~TalentInfoNode()
{
    removeAllChildren();
}

bool TalentInfoNode::init(const Callback& callback)
{
    if (Node::init()) {
        _callback = callback;
        
        static const Size size(420, 570);
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        
        static const float edgeY(8.0f);
        for (int i = 0; i < TalentsMaxCount; ++i) {
            static const float offsetY(25.0f);
            auto node = TalentBriefNode::create();
            const auto& nodeSize(node->getContentSize());
            node->setPosition(Point(size.width / 2, size.height - (edgeY + nodeSize.height * (i + 0.5f) + offsetY * i)));
            addChild(node);
            _nodes.push_back(node);
        }
        
        static const string file("GameImages/public/button_yellow.png");
        auto button = Button::create(file);
        button->addClickEventListener([this](Ref*) {
            if (_callback && _name.size() > 0) {
                _callback(_name, _cost);
            }
        });
        const auto& buttonSize(button->getContentSize());
        button->setPosition(Point(size.width / 2, edgeY + buttonSize.height / 2));
        addChild(button);
        
        _costLabel = CocosUtils::createLabel("cost", DEFAULT_FONT_SIZE);
        _costLabel->setPosition(Point(buttonSize.width / 2, buttonSize.height / 2));
        button->addChild(_costLabel);
        
        updateCost(0);
        
        return true;
    }
    
    return false;
}

void TalentInfoNode::update(const string& name)
{
    _name = name;
    
    // TODO:
    int cost = 10;
    updateCost(cost);
    
    int cnt = 4;
    const size_t nodesCnt(_nodes.size());
    for (int i = 0; i < TalentsMaxCount; ++i) {
        if (nodesCnt > i) {
            auto node = _nodes.at(i);
            if (i < cnt) {
                node->setVisible(true);
                node->update("name", 1, "description");
            } else {
                node->setVisible(false);
            }
        }
    }
}

void TalentInfoNode::updateCost(int count)
{
    _cost = count;
    if (_costLabel) {
        _costLabel->setString(StringUtils::format("Cost: %d", count));
    }
}