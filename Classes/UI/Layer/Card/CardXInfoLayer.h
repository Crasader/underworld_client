//
//  CardXInfoLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/6/29.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardXInfoLayer_h
#define CardXInfoLayer_h

#include "cocos2d.h"

USING_NS_CC;

class CardInfoNode;
class TalentInfoNode;

class CardXInfoLayerObserver
{
public:
    virtual ~CardXInfoLayerObserver() {}
    virtual void onCardXInfoLayerClosed(const std::string& name) = 0;
    virtual void onCardXInfoLayerUpgradeCard(const std::string& name) = 0;
    virtual void onCardXInfoLayerUpgradeTalent(const std::string& name) = 0;
};

class CardXInfoLayer : public Layer
{
public:
    static CardXInfoLayer* create();
    virtual ~CardXInfoLayer();
    void registerObserver(CardXInfoLayerObserver *observer);
    void update(const std::string& name);
    
protected:
    CardXInfoLayer();
    virtual bool init() override;
    
    // LayerColor
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
private:
    CardXInfoLayerObserver *_observer;
    std::string _name;
    CardInfoNode* _cardInfoNode;
    TalentInfoNode* _talentInfoNode;
    Label* _titleLabel;
};

#endif /* CardXInfoLayer_h */
