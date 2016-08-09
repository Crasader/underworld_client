//
//  LanguageConfirmationLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/8/9.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef LanguageConfirmationLayer_h
#define LanguageConfirmationLayer_h

#include "cocos2d.h"

USING_NS_CC;

class LanguageConfirmationLayerObserver
{
public:
    virtual ~LanguageConfirmationLayerObserver() {}
    virtual void onLanguageConfirmationLayerConfirm(ssize_t idx) = 0;
};

class LanguageConfirmationLayer : public LayerColor
{
public:
    static LanguageConfirmationLayer* create(ssize_t idx);
    virtual ~LanguageConfirmationLayer();
    void registerObserver(LanguageConfirmationLayerObserver *observer);
    
protected:
    LanguageConfirmationLayer();
    
    // LayerColor
    bool init(ssize_t idx);
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
private:
    LanguageConfirmationLayerObserver *_observer;
    ssize_t _idx;
};

#endif /* LanguageConfirmationLayer_h */
