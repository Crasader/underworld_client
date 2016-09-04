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

UniversalButton* BattleDeckUI::createButton(CardOpType opType, const function<void()>& callback)
{
    static const UniversalButton::BSize size(UniversalButton::BSize::Small);
    
    UniversalButton::BType type(UniversalButton::BType::Blue);
    string title;
    switch (opType) {
        case CardOpType::Upgrade: {
            type = UniversalButton::BType::Green;
            title = LocalHelper::getString("ui_deck_upgrade");
        }
            break;
        case CardOpType::Use: {
            type = UniversalButton::BType::Green;
            title = LocalHelper::getString("ui_deck_use");
        }
            break;
        case CardOpType::Info: {
            title = LocalHelper::getString("ui_deck_info");
        }
            break;
        case CardOpType::Move: {
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

void BattleDeckUI::readdChild(Node* parent, Node* child)
{
    if (child && parent) {
        auto cp(child->getParent());
        if (cp != parent) {
            Point worldPoint(child->getPosition());
            if (cp) {
                child->retain();
                worldPoint = cp->convertToWorldSpace(worldPoint);
                child->removeFromParent();
            }
            parent->addChild(child);
            
            // reset position after "addChild",
            // because the parent of "child" may not be "parent"
            if (cp) {
                child->setPosition(child->getParent()->convertToNodeSpace(worldPoint));
                child->release();
            }
        }
    }
}
