//
//  BattleDeckUI.h
//  Underworld_Client
//
//  Created by Andy on 16/8/10.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef BattleDeckUI_h
#define BattleDeckUI_h

#include "cocos2d.h"

USING_NS_CC;

enum class CardOpType {
    Upgrade,
    Use,
    Info,
    Move
};

class UniversalButton;

namespace BattleDeckUI
{
    std::string getResourcePath(const std::string& file);
    UniversalButton* createButton(CardOpType opType, const std::function<void()>& callback);
    void readdChild(Node* parent, Node* child);
}

#endif /* BattleDeckUI_h */
