//
//  ResourceButton.h
//  Underworld_Client
//
//  Created by Andy on 15/7/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ResourceButton_h
#define ResourceButton_h

#include "XButton.h"
#include "CocosGlobal.h"

class ResourceButton : public XButton
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
    virtual void setEnabled(bool enabled) override;
    
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
    ResourceType _type;
    int _count;
    Color4B _color;
    Sprite* _icon;
    Node* _iconNode;
    Label* _countLabel;
};

#endif /* ResourceButton_h */
