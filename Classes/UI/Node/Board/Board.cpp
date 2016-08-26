//
//  Board.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "Board.h"
#include "CocosUtils.h"
#include "PureScale9Sprite.h"

using namespace std;

Board* Board::create(int blocks)
{
    auto ret = new (nothrow) Board();
    if (ret && ret->init(blocks)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

Board::Board() {}

Board::~Board() {}

bool Board::init(int blocks)
{
    if (UniversalBoard::init("ui_background_5.png")) {
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
        
        return true;
    }
    
    return false;
}

Node* Board::getSubNode(int idx) const
{
    if (_subNodes.size() > idx) {
        return _subNodes.at(idx);
    }
    
    return nullptr;
}
