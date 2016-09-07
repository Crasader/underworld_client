//
//  GuildGenericCell.h
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef GuildGenericCell_h
#define GuildGenericCell_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class GuildGenericCell: public ui::Button
{
public:
    virtual ~GuildGenericCell() = 0;
    
protected:
    GuildGenericCell();
    virtual bool init() override;
};

#endif /* GuildGenericCell_h */
