//
//  MessageBoxLayer.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef MessageBoxLayer_h
#define MessageBoxLayer_h

#include "MessageBoxBaseLayer.h"
#include "CocosGlobal.h"
#include "LocalHelper.h"

#define MESSAGEBOX_DEFAULT_FONT_SIZE (BIG_FONT_SIZE)

enum class MessageBoxType {
    OnlyYes = 0,
    YesNo,
    NoYes
};

class MessageBoxLayer : public MessageBoxBaseLayer
{
public:
    static MessageBoxLayer * getInstance();
    static void purge();
    void show(const std::string& message, int fontSize = MESSAGEBOX_DEFAULT_FONT_SIZE);
    void show(const std::string& message, MessageBoxType type, const Button::ccWidgetClickCallback& confirmCallback, const Button::ccWidgetClickCallback& cancelCallback = nullptr, bool autoHide = true, const std::string& confirmString = LocalHelper::getString("hint_confirm"), const std::string& cancelString = LocalHelper::getString("hint_cancel"));
    void hide();
    
protected:
    MessageBoxLayer();
    virtual ~MessageBoxLayer();
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    void resetMessageLabel(int fontSize = MESSAGEBOX_DEFAULT_FONT_SIZE);
    
private:
    MessageBoxType _type;
    bool _autoHide;
    Button::ccWidgetClickCallback _confirmCallback;
    Button::ccWidgetClickCallback _cancelCallback;
    Label *_messageLabel;
    Point _messageLabelPosition;
};

#endif /* MessageBoxLayer_h */
