//
//  ResourceButton.h
//  Underworld_Client
//
//  Created by Andy on 15/7/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ResourceButton_h
#define ResourceButton_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "CocosGlobal.h"

USING_NS_CC;

class XButton;

class ResourceButton : public Node
{
public:
    static ResourceButton* create(bool isBigSize, bool animated, ResourceType type, int count, const Color4B& color, const ui::Widget::ccWidgetClickCallback& callback);
    virtual ~ResourceButton();
    
    ResourceType getType() const;
    int getCount() const;
    bool isResourceEnough() const;
    bool isEnabled() const;
    
    void setType(ResourceType type);
    void setCount(int count);
    void setResourceEnough(bool enable);
    void setEnabled(bool enable);
    void setClickEventListener(const ui::Widget::ccWidgetClickCallback& callback);
    
protected:
    ResourceButton();
    bool init(bool isBigSize, bool animated, ResourceType type, int count, const Color4B& color, const ui::Widget::ccWidgetClickCallback& callback);
    void addIconNode(ResourceType type);
    const Color4B& getEnabledColor() const;
    void resize();
    
private:
    bool _isBigSize;
    bool _animated;
    bool _resourceEnough;
    bool _enabled;
    ResourceType _type;
    int _count;
    Color4B _color;
    Sprite* _icon;
    Node* _iconNode;
    Label* _countLabel;
    XButton *_button;
};

#endif /* ResourceButton_h */