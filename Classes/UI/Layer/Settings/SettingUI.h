//
//  SettingUI.h
//  Underworld_Client
//
//  Created by Andy on 16/7/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef SettingUI_h
#define SettingUI_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

namespace SettingUI
{
    enum class ButtonType {
        Normal,
        Selected
    };
    
    std::string getResourcePath(const std::string& file);
    Node* createSubBackground(const Size& size);
    ui::Button* createButton(ButtonType type, const std::string& title);
    void updateButton(ui::Button* button, ButtonType type, const std::string& title);
    Node* createExitButton(Node* parent, const std::function<void()>& callback);
}

#endif /* SettingUI_h */
