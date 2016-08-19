//
//  BattleDeckUI.h
//  Underworld_Client
//
//  Created by Andy on 16/8/10.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef BattleDeckUI_h
#define BattleDeckUI_h

#include <string>
#include <functional>

enum class DeckCardOpType {
    Upgrade,
    Use,
    Info,
    Move
};

class UniversalButton;

namespace BattleDeckUI
{
    std::string getResourcePath(const std::string& file);
    UniversalButton* createButton(DeckCardOpType opType, const std::function<void()>& callback);
}

#endif /* BattleDeckUI_h */
