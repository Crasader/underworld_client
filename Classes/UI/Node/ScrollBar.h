//
//  ScrollBar.h
//  Underworld_Client
//
//  Created by Andy on 15/6/23.
//
//

#ifndef ScrollBar__
#define ScrollBar__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"

USING_NS_CC;

class ScrollBar : public Layer
{
public:
    virtual ~ScrollBar();
    static ScrollBar * create(ui::Scale9Sprite * bar_bg, ui::Scale9Sprite * bar_slider, extension::TableView * tableView, extension::ScrollView::Direction dir);
    static ScrollBar * create(const char * bar_bgFile, const char * bar_sliderFile, extension::TableView * tableView, extension::ScrollView::Direction dir);
    
protected:
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchMoved(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    virtual void update(float dt) override;
    
    ScrollBar();
    bool init(ui::Scale9Sprite * bar_bg, ui::Scale9Sprite * bar_slider, extension::TableView * tableView, extension::ScrollView::Direction dir);
    void updateSlider();
    
private:
    extension::TableView * m_pTarget;
    ui::Scale9Sprite * m_pBg;
    ui::Scale9Sprite * m_pSlider;
    extension::ScrollView::Direction m_direction;
    Size m_preContentSize;
    Size m_viewSize;
    bool m_sliderTouched;
    Vec2 m_firstTouch;
    Vec2 m_sliderCurPos;
    Vec2 m_targetCurPos;
};

#endif /* defined(ScrollBar__) */