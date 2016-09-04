//
//  SeniorCard.h
//  Underworld_Client
//
//  Created by Andy on 16/8/29.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef SeniorCard_h
#define SeniorCard_h

#include "JuniorCard.h"

class ResourceButton;

class SeniorCard : public JuniorCard
{
public:
    static SeniorCard* create();
    virtual ~SeniorCard();
    
private:
    SeniorCard();
    virtual bool init() override;
    virtual void updateProperty(const AbstractProperty* property) final;
    virtual void updateData(const AbstractData* data) final;
    
private:
    Label* _name;
    ResourceButton* _button;
};

#endif /* SeniorCard_h */
