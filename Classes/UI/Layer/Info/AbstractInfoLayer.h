//
//  AbstractInfoLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/9/5.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef AbstractInfoLayer_h
#define AbstractInfoLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class UniversalBoard;
class JuniorCard;
class ResourceButton;
class CardAttributeNode;
class AbstractData;
class DevelopProperty;

class AbstractInfoLayer : public LayerColor
{
public:
    virtual ~AbstractInfoLayer();
    
protected:
    AbstractInfoLayer();
    bool init(int cardId, const AbstractData* data);
    virtual void initUI() = 0;
    virtual void updateProperty(const DevelopProperty* property);
    virtual void updateData(const AbstractData* data);
    
private:
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    void update(int cardId, const AbstractData* data);
    
protected:
    UniversalBoard* _board;
    JuniorCard* _icon;
    Label* _description;
    ResourceButton* _resourceButton;
    std::vector<CardAttributeNode*> _attributes;
    const AbstractData* _data;
    const DevelopProperty* _property;
};

#endif /* AbstractInfoLayer_h */
