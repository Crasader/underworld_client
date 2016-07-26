//
//  SettingsLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/7/25.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef SettingsLayer_h
#define SettingsLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "LanguageLayer.h"
#include "RenameLayer.h"

USING_NS_CC;

class SettingsLayerObserver
{
public:
    virtual ~SettingsLayerObserver() {}
};

enum class SettingType;

class SettingsLayer
: public LayerColor
, public LanguageLayerObserver
, public RenameLayerObserver
{
public:
    static SettingsLayer* create();
    virtual ~SettingsLayer();
    void registerObserver(SettingsLayerObserver *observer);
    
protected:
    SettingsLayer();
    
    // LayerColor
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // LanguageLayerObserver
    virtual void onLanguageLayerReturn() override;
    virtual void onLanguageLayerClosed() override;
    
    // RenameLayerObserver
    
    ui::Button* createReturnButton(Node* parent, const Vec2& offset, const std::function<void()>& callback) const;
    Node* createContent(Node* parent);
    void createSettingNodes(Node* parent, const std::vector<SettingType>& types, const Point& basePoint, int row, int column, const Vec2& edge, const Vec2& space);
    void checkButtonStatus(SettingType type, bool& isOn, bool& isEnabled) const;
    void onButtonClicked(SettingType type);
    
private:
    SettingsLayerObserver *_observer;
    ui::Button* _returnButton;
};

#endif /* SettingsLayer_h */
