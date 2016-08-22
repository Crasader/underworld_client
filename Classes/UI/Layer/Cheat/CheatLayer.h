//
//  CheatLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/8/4.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CheatLayer_h
#define CheatLayer_h

#include "cocos2d.h"

USING_NS_CC;

class CheatLayer : public LayerColor
{
public:
    static CheatLayer* getInstance();
    static void purge();
    void show(Node* parent);
    
protected:
    CheatLayer();
    virtual ~CheatLayer();
    bool init(const std::function<void(Ref*)>& callback);
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    void setNodePosition(int idx, Node* node);
    void setLoadPVR();
    
private:
    Node* _bg;
    Label* _loadPVRLabel;
};

#endif /* CheatLayer_h */
