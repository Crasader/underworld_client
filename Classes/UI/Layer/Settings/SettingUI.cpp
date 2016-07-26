//
//  SettingUI.cpp
//  Underworld_Client
//
//  Created by Andy on 16/7/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "SettingUI.h"
#include "CocosUtils.h"

using namespace std;

#pragma mark - private
namespace SettingUI {
    string getButtonFile(ButtonType type) {
        if (ButtonType::Normal == type) {
            return getResourcePath("button_lanse_1.png");
        } else if (ButtonType::Selected == type) {
            return getResourcePath("button_hongse_1.png");
        }
        
        return "";
    }
}

#pragma mark - public
std::string SettingUI::getResourcePath(const std::string& file)
{
    return "GameImages/settings_ui/" + file;
}

Node* SettingUI::createSubBackground(const Size& size)
{
    auto node = Node::create();
    node->setAnchorPoint(Point::ANCHOR_MIDDLE);
    node->setContentSize(size);
    
    auto s = Sprite::create(getResourcePath("ui_background_1.png"));
    s->setScale(size.width, size.height);
    s->setPosition(Point(size.width / 2, size.height / 2));
    node->addChild(s);
    
    return node;
}

ui::Button* SettingUI::createButton(ButtonType type, const std::string& title)
{
    const auto& file = getButtonFile(type);
    if (!file.empty()) {
        const auto disable(getResourcePath("button_huise.png"));
        auto button = ui::Button::create(file, file, disable);
        const auto& size(button->getContentSize());
        auto label = CocosUtils::createLabel(title, DEFAULT_FONT_SIZE);
        label->setAnchorPoint(Point::ANCHOR_MIDDLE);
        label->setPosition(Point(size.width / 2, size.height / 2));
        label->setTextColor(Color4B::BLACK);
        button->addChild(label);
        
        return button;
    }
    
    return nullptr;
}

void SettingUI::updateButton(ui::Button* button, ButtonType type, const std::string& title)
{
    if (button) {
        const auto& file = getButtonFile(type);
        if (!file.empty()) {
            button->loadTextures(file, file);
            if (button->getChildrenCount() > 0) {
                auto child = button->getChildren().front();
                if (child) {
                    auto label = dynamic_cast<Label*>(child);
                    if (label) {
                        label->setString(title);
                    }
                }
            }
        }
    }
}

Node* SettingUI::createExitButton(Node* parent, const std::function<void()>& callback)
{
    return CocosUtils::createExitButton(parent, callback, getResourcePath("button_hongse.png"));
}
