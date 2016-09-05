//
//  JuniorCard.h
//  Underworld_Client
//
//  Created by Andy on 16/8/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef JuniorCard_h
#define JuniorCard_h

#include "BaseCard.h"

class JuniorCard : public BaseCard
{
public:
    static JuniorCard* create();
    virtual ~JuniorCard();
    
    bool canUpgrade() const;
    
protected:
    JuniorCard();
    virtual bool init() override;
    virtual void updateData(const AbstractData* data) override;
    
protected:
    Node* _ptBackground;
    ProgressTimer* _pt;
    Label* _amount;
    Sprite* _arrow;
};

#endif /* JuniorCard_h */
