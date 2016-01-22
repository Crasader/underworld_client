//
//  ResourceButton.h
//  ZombiePop
//
//  Created by Andy on 15/7/14.
//
//

#ifndef ResourceButton__
#define ResourceButton__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "CocosGlobal.h"

USING_NS_CC;
using namespace ui;

class ResourceButton : public Node
{
public:
    static ResourceButton* create(bool isBigSize, bool animated, bool needResize, ResourceType type, int count, const Color4B& color, const Button::ccWidgetClickCallback& callback);
    virtual ~ResourceButton();
    ResourceType getType() const;
    int getCount() const;
    void setType(ResourceType type);
    void setCount(int count);
    void setEnabled(bool enable);
    void setTouchEventListener(const Button::ccWidgetTouchCallback& callback);
    void setClickEventListener(const Button::ccWidgetClickCallback& callback);
    
protected:
    ResourceButton();
    bool init(bool isBigSize, bool animated, bool needResize, ResourceType type, int count, const Color4B& color, const Button::ccWidgetClickCallback& callback);
    void addIconNode(ResourceType type);
    void resize();
    
private:
    bool _isBigSize;
    bool _animated;
    bool _needResize;
    bool _enabled;
    ResourceType _type;
    int _count;
    Color4B _color;
    Sprite *_icon;
    Node* _iconNode;
    Label *_countLabel;
    Button *_button;
};

#endif /* defined(__ZombiePop__ResourceButton__) */