//
//  RuneBagLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef RuneBagLayer_h
#define RuneBagLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "RuneNode.h"

USING_NS_CC;

class RuneData;

class RuneBagLayerObserver
{
public:
    virtual ~RuneBagLayerObserver() {}
};

class RuneBagLayer
: public LayerColor
, public RuneNodeObserver
{
public:
    static RuneBagLayer* create();
    virtual ~RuneBagLayer();
    void registerObserver(RuneBagLayerObserver *observer);
    
    void update(const RuneData* data);
    
private:
    RuneBagLayer();
    
    // LayerColor
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // RuneNodeObserver
    virtual void onRuneNodeClicked(RuneNode* pSender) override;
    
    void initRunes();
    float getHeight(size_t count, float spaceY) const;
    Point getPosition(int row, int column) const;
    
private:
    RuneBagLayerObserver* _observer;
    
    // UI
    Label* _name;
    Label* _description;
    Label* _effect;
    ui::ScrollView* _scrollView;
    
    // Data
    const RuneData* _data;
    Size _nodeSize;
    float _nodeSpaceX;
    Size _scrollViewMinSize;
};

#endif /* RuneBagLayer_h */
