//
//  TradeableCard.h
//  Underworld_Client
//
//  Created by Andy on 16/9/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef TradeableCard_h
#define TradeableCard_h

#include "BaseCard.h"

class ResourceButton;

class TradeableCard : public BaseCard
{
public:
    static TradeableCard* create();
    virtual ~TradeableCard();
    
protected:
    TradeableCard();
    virtual bool init() override;
    virtual void updateData(const AbstractData* data) override;
    
protected:
    ResourceButton* _button;
};

#endif /* TradeableCard_h */
