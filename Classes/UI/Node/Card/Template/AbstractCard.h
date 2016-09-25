//
//  AbstractCard.h
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef AbstractCard_h
#define AbstractCard_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class AbstractData;
class AbstractProperty;

class AbstractCard : public ui::Widget
{
public:
    virtual ~AbstractCard();
    
    bool update(int cardId, const AbstractData* data);
    int getCardId() const;
    const AbstractData* getCardData() const;
    
protected:
    AbstractCard();
    virtual void updateProperty(const AbstractProperty* property) = 0;
    virtual void updateData(const AbstractData* data);
    
protected:
    ui::ImageView* _icon;
    Label* _level;
    Sprite* _qualityBox;
    int _cardId;
    const AbstractData* _data;
};

#endif /* AbstractCard_h */
