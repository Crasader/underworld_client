//
//  SettingUI.cpp
//  Underworld_Client
//
//  Created by Andy on 16/7/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "SettingUI.h"
#include "CocosUtils.h"

std::string SettingUI::getResourcePath(const std::string& file)
{
    return "GameImages/settings_ui/" + file;
}

Node* SettingUI::createExitButton(Node* parent, const std::function<void()>& callback)
{
    return CocosUtils::createExitButton(parent, callback, getResourcePath("button_hongse.png"));
}
