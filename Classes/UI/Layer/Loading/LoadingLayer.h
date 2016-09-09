//
//  LoadingLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/9/9.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef LoadingLayer_h
#define LoadingLayer_h

#include "cocos2d.h"

USING_NS_CC;

class LoadingLayer : public LayerColor
{
public:
    typedef std::function<void(Node*)> Callback;
    
    static LoadingLayer* create(const Callback& callback);
    virtual ~LoadingLayer();
    
protected:
    LoadingLayer();
    
    // LayerColor
    bool init(const Callback& callback);
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    void loadResources();
    void loadResource();
    void onResourcesLoaded();
    
private:
    ProgressTimer* _progressTimer;
    Label* _hint;
    Callback _callback;
    bool _isLoading;
    int _loaded;
};

#endif /* LoadingLayer_h */
