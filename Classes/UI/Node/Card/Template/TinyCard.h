//
//  TinyCard.h
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef TinyCard_h
#define TinyCard_h

#include "AbstractCard.h"

class TinyCard : public AbstractCard
{
public:
    static constexpr float Width = 53;
    static constexpr float Height = 68;
    typedef std::function<void()> Callback;
    static TinyCard* create();
    virtual ~TinyCard();
    
    void setCallback(const Callback& callback);
    
protected:
    TinyCard();
    virtual bool init() override;
    virtual void updateProperty(const AbstractProperty* property) override;
    virtual void updateData(const AbstractData* data) override;
    
protected:
    Callback _callback;
    bool _touchInvalid;
};

#endif /* TinyCard_h */
