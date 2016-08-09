//
//  RenameLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/7/25.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef RenameLayer_h
#define RenameLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class RenameLayerObserver
{
public:
    virtual ~RenameLayerObserver() {}
};

class RenameLayer
: public LayerColor
, public ui::EditBoxDelegate
{
public:
    static RenameLayer* create();
    virtual ~RenameLayer();
    void registerObserver(RenameLayerObserver *observer);
    
protected:
    RenameLayer();
    
    // LayerColor
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // EditBoxDelegate
    virtual void editBoxReturn(ui::EditBox* editBox) override;
    
private:
    RenameLayerObserver *_observer;
};

#endif /* RenameLayer_h */
