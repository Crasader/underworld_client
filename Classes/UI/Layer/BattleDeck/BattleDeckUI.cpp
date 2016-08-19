//
//  BattleDeckUI.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/10.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "BattleDeckUI.h"
#include "UniversalButton.h"
#include "LocalHelper.h"

using namespace std;

string BattleDeckUI::getResourcePath(const string& file)
{
    return "GameImages/battle_deck_ui/" + file;
}

UniversalButton* BattleDeckUI::createButton(DeckCardOpType opType, const function<void()>& callback)
{
    static const UniversalButton::BSize size(UniversalButton::BSize::Small);
    
    UniversalButton::BType type(UniversalButton::BType::Blue);
    string title;
    switch (opType) {
        case DeckCardOpType::Upgrade: {
            type = UniversalButton::BType::Green;
            title = LocalHelper::getString("ui_deck_upgrade");
        }
            break;
        case DeckCardOpType::Use: {
            type = UniversalButton::BType::Green;
            title = LocalHelper::getString("ui_deck_use");
        }
            break;
        case DeckCardOpType::Info: {
            title = LocalHelper::getString("ui_deck_info");
        }
            break;
        case DeckCardOpType::Move: {
            type = UniversalButton::BType::Green;
            title = LocalHelper::getString("ui_deck_move");
        }
            break;
        default:
            break;
    }
    
    auto button = UniversalButton::create(size, type, title);
    button->setCallback([=](Ref*) {
        if (callback) {
            callback();
        }
    });
    return button;
}
