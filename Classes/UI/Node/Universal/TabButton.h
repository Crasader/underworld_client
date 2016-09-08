//
//  TabButton.h
//  Underworld_Client
//
//  Created by Andy on 15/12/17.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef TabButton_h
#define TabButton_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class TabButton : public ui::Button
{
public:
    static TabButton* create(const std::string& title, const std::string& normal, const std::string& selected, const ui::Button::ccWidgetClickCallback& callback);
    static TabButton* create(const std::string& title, const ui::Button::ccWidgetClickCallback& callback);
    virtual ~TabButton();
    virtual void setEnabled(bool enabled) override;
    
protected:
    TabButton();
    bool init(const std::string& title, const std::string& normal, const std::string& selected, const ui::Button::ccWidgetClickCallback& callback);
    bool init(const std::string& title, const ui::Button::ccWidgetClickCallback& callback);
};

#endif /* TabButton_h */
