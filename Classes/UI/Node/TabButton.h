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

class TabButton : public Node
{
public:
    static TabButton* create(const std::string& title, const std::string& normal, const std::string& selected, const ui::Button::ccWidgetClickCallback& callback);
    static TabButton* create(const std::string& title, const ui::Button::ccWidgetClickCallback& callback);
    virtual ~TabButton();
    void setEnabled(bool enabled);
    
protected:
    TabButton();
    bool init(const std::string& title, const std::string& normal, const std::string& selected, const ui::Button::ccWidgetClickCallback& callback);
    bool init(const std::string& title, const ui::Button::ccWidgetClickCallback& callback);
    
private:
    ui::Button* _button;
};

#endif /* TabButton_h */
