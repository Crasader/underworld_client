//
//  DevelopCard.h
//  Underworld_Client
//
//  Created by Andy on 16/8/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef DevelopCard_h
#define DevelopCard_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class DeckCard;

class DevelopCardObserver
{
public:
    virtual ~DevelopCardObserver() {}
    virtual void onDevelopCardClicked() = 0;
};

#endif /* DevelopCard_h */
