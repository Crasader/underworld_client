//
//  GuildNoticeLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef GuildNoticeLayer_h
#define GuildNoticeLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class GuildNoticeLayerObserver
{
public:
    virtual ~GuildNoticeLayerObserver() {}
    virtual void onGuildNoticeLayerSend(Node* pSender, const std::string& title, const std::string& notice) = 0;
};

class GuildNoticeLayer
: public LayerColor
, public ui::EditBoxDelegate
{
public:
    static GuildNoticeLayer* create();
    virtual ~GuildNoticeLayer();
    void registerObserver(GuildNoticeLayerObserver *observer);
    
protected:
    GuildNoticeLayer();
    
    // LayerColor
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // EditBoxDelegate
    virtual void editBoxReturn(ui::EditBox* editBox) override;
    
private:
    GuildNoticeLayerObserver* _observer;
    ui::EditBox* _noticeTitle;
    ui::EditBox* _notice;
};

#endif /* GuildNoticeLayer_h */
