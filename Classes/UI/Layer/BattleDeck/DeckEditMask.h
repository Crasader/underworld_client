//
//  DeckEditMask.h
//  Underworld_Client
//
//  Created by Andy on 16/8/15.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef DeckEditMask_h
#define DeckEditMask_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class DeckEditMaskObserver
{
public:
    virtual ~DeckEditMaskObserver() {}
    virtual void onDeckEditMaskTouched(const Point& point) = 0;
};

class DeckEditMask : public LayerColor
{
public:
    static DeckEditMask* create(const Size& size);
    virtual ~DeckEditMask();
    void registerObserver(DeckEditMaskObserver* observer);
    
private:
    DeckEditMask();
    bool init(const Size& size);
    
    // LayerColor
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
private:
    DeckEditMaskObserver* _observer;
};

#endif /* DeckEditMask_h */
