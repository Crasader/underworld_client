//
//  EditDeckMask.h
//  Underworld_Client
//
//  Created by Andy on 16/8/15.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef EditDeckMask_h
#define EditDeckMask_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class EditDeckMaskObserver
{
public:
    virtual ~EditDeckMaskObserver() {}
    virtual void onEditDeckMaskTouched(const Point& point) = 0;
};

class EditDeckMask : public LayerColor
{
public:
    static EditDeckMask* create(const Size& size);
    virtual ~EditDeckMask();
    void registerObserver(EditDeckMaskObserver* observer);
    
private:
    EditDeckMask();
    bool init(const Size& size);
    
    // LayerColor
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
private:
    EditDeckMaskObserver* _observer;
};

#endif /* EditDeckMask_h */
