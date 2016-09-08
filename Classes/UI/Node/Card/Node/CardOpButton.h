//
//  CardOpButton.h
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardOpButton_h
#define CardOpButton_h

#include "XButton.h"
#include "CocosGlobal.h"

class CardOpButton : public XButton
{
public:
    static CardOpButton* create(CardOpType opType);
    virtual ~CardOpButton();
    
protected:
    CardOpButton();
    bool init(CardOpType opType);
};

#endif /* CardOpButton_h */
