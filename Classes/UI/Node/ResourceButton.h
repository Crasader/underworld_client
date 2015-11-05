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
    static ResourceButton* create(bool isBigSize, ResourceType type, int count, const Button::ccWidgetClickCallback& callback);
    virtual ~ResourceButton();
    ResourceType getType() const;
    int getCount() const;
    void setType(ResourceType type);
    void setCount(int count);
    void setTouchEventListener(const Button::ccWidgetTouchCallback& callback);
    void setClickEventListener(const Button::ccWidgetClickCallback& callback);
    
protected:
    ResourceButton();
    bool init(bool isBigSize, ResourceType type, int count, const Button::ccWidgetClickCallback& callback);
    void resize();
    
private:
    ResourceType _type;
    int _count;
    Sprite *_icon;
    LabelAtlas *_countLabel;
    Button *_button;
};

#endif /* defined(__ZombiePop__ResourceButton__) */