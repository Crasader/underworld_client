//
//  UniversalBoard.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "UniversalBoard.h"
#include "CocosUtils.h"
#include "PureScale9Sprite.h"

using namespace std;

UniversalBoard* UniversalBoard::create(int blocks)
{
    auto ret = new (nothrow) UniversalBoard();
    if (ret && ret->init(blocks)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

UniversalBoard::UniversalBoard()
:_title(nullptr)
,_exitButton(nullptr)
,_callback(nullptr) {}

UniversalBoard::~UniversalBoard()
{
    removeAllChildren();
}

bool UniversalBoard::init(int blocks)
{
    if (Sprite::init()) {
        setTexture(CocosUtils::getResourcePath("ui_background_5.png"));
        
        const auto& size(getContentSize());
        static const Size subSize(992, 513);
        const float edge((size.width - subSize.width) / 2);
        const float posY(subSize.height / 2 + edge);
        
        if (blocks > 0) {
            vector<float> widths = vector<float>();
            if (1 == blocks) {
                widths.push_back(subSize.width);
            } else if (2 == blocks) {
                widths.push_back(366);
                widths.push_back(612);
            }
            
            float totalWidth(0);
            for (float width : widths) {
                totalWidth += width;
            }
            
            const float space(blocks > 1 ? MAX(0, (subSize.width - totalWidth) / (blocks - 1)) : 0);
            float x(edge);
            for (int i = 0; i < widths.size(); ++i) {
                const float width(widths.at(i));
                auto subNode = PureScale9Sprite::create(PureScale9Sprite::Type::BlueLight);
                subNode->setContentSize(Size(width, subSize.height));
                subNode->setPosition(x + width / 2, posY);
                addChild(subNode);
                _subNodes.push_back(subNode);
                
                x += width + space;
            }
        }
        
        _exitButton = CocosUtils::createRedExitButton(this, [this]() {
            if (_callback) {
                _callback();
            } else if (_parent) {
                _parent->removeFromParent();
            } else {
                removeFromParent();
            }
        });
        
        auto title = CocosUtils::createLabel("Untitled", TITLE_FONT_SIZE);
        title->setAnchorPoint(Point::ANCHOR_MIDDLE);
        title->setPosition(Point(size.width / 2, (size.height + subSize.height + edge) / 2));
        addChild(title);
        _title = title;
        
        return true;
    }
    
    return false;
}

void UniversalBoard::setTitle(const string& title)
{
    if (_title) {
        _title->setString(title);
    }
}

void UniversalBoard::setExitCallback(const Callback& callback)
{
    if (_exitButton) {
        _callback = callback;
    }
}

Node* UniversalBoard::getSubNode(int idx) const
{
    if (_subNodes.size() > idx) {
        return _subNodes.at(idx);
    }
    
    return nullptr;
}
